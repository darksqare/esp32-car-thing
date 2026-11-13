#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "config.h"
#include "storage.h"
#include "hardware.h"
#include "web_ui.h"

namespace {
    const IPAddress AP_IP(192, 168, 4, 1);
    const IPAddress AP_NETMASK(255, 255, 255, 0);
    constexpr uint16_t DNS_PORT = 53;

    DNSServer dnsServer;
    AsyncWebServer server(80);
    AsyncWebSocket ws("/ws");

    CarSettings settings;
    CarState state{};

    uint32_t lastTelMs = 0;
    bool pendingReboot = false;
    uint32_t rebootAtMs = 0;

    void appendSettingsJson(JsonObject obj) {
        obj["invertSteering"] = settings.invertSteering;
        obj["invertMotor"]    = settings.invertMotor;
        obj["motorMinSpeed"]  = settings.motorMinSpeed;
        obj["motorMaxSpeed"]  = settings.motorMaxSpeed;
        obj["failsafeMs"]     = settings.failsafeMs;
        obj["apSsid"]         = settings.apSsid;
        obj["apPass"]         = settings.apPass;
    }

    void appendStateJson(JsonObject obj) {
        obj["throttleInput"]  = state.throttleInput;
        obj["steerInput"]     = state.steerInput;
        obj["motorPwm"]       = state.motorPwm;
        obj["motorDir"]       = state.motorDir;
        obj["servoAngle"]     = state.servoAngle;
        obj["failsafeActive"] = state.failsafeActive;
        obj["wifiClients"]    = WiFi.softAPgetStationNum();
    }

    void applyControlCommand(int16_t thr, int16_t steer) {
        state.throttleInput  = (int8_t)constrain(thr, -100, 100);
        state.steerInput     = (int8_t)constrain(steer, -100, 100);
        state.lastControlMs  = millis();
        state.failsafeActive = false;
        Hardware::applyOutputs(settings, state);
    }

    void broadcastTelemetry() {
        if (ws.count() == 0) return;

        JsonDocument doc;
        doc["type"] = "tel";
        doc["t"]    = state.throttleInput;
        doc["dir"]  = state.motorDir;
        doc["pwm"]  = state.motorPwm;
        doc["ang"]  = state.servoAngle;
        doc["fs"]   = state.failsafeActive;
        doc["cli"]  = WiFi.softAPgetStationNum();

        String out;
        serializeJson(doc, out);
        ws.textAll(out);
    }

    void handleWsMessage(void* arg, uint8_t* data, size_t len) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (!info->final || info->index != 0 || info->len != len || info->opcode != WS_TEXT) {
            return;
        }

        // Fast-path compact command: "C:<throttle>,<steer>"
        if (len >= 4 && data[0] == 'C' && data[1] == ':') {
            char buf[32];
            size_t copyLen = min(len, sizeof(buf) - 1);
            memcpy(buf, data, copyLen);
            buf[copyLen] = '\0';

            char* comma = strchr(buf + 2, ',');
            if (comma != nullptr) {
                *comma = '\0';
                int16_t thr = (int16_t)atoi(buf + 2);
                int16_t str = (int16_t)atoi(comma + 1);
                applyControlCommand(thr, str);
            }
            return;
        }

        // JSON fallback: {"t": 50, "s": -30}
        JsonDocument doc;
        if (!deserializeJson(doc, data, len)) {
            int16_t thr = doc["t"] | 0;
            int16_t str = doc["s"] | 0;
            applyControlCommand(thr, str);
        }
    }

    void onWsEvent(AsyncWebSocket* serverPtr, AsyncWebSocketClient* client,
                   AwsEventType type, void* arg, uint8_t* data, size_t len) {
        (void)serverPtr;
        switch (type) {
            case WS_EVT_CONNECT:
                Serial.printf("[WS] Client #%u connected\n", client->id());
                broadcastTelemetry();
                break;
            case WS_EVT_DISCONNECT:
                Serial.printf("[WS] Client #%u disconnected\n", client->id());
                if (ws.count() == 0) {
                    Hardware::emergencyStop(settings, state);
                    state.failsafeActive = true;
                }
                break;
            case WS_EVT_DATA:
                handleWsMessage(arg, data, len);
                break;
            case WS_EVT_ERROR:
            case WS_EVT_PONG:
                break;
        }
    }

    void setupWiFiAP() {
        WiFi.persistent(false);
        WiFi.mode(WIFI_AP);
        delay(100);

        bool ok = false;
        if (strlen(settings.apPass) >= 8) {
            ok = WiFi.softAP(settings.apSsid, settings.apPass);
        } else {
            ok = WiFi.softAP(settings.apSsid);
        }
        delay(100);

        WiFi.softAPConfig(AP_IP, AP_IP, AP_NETMASK);
        WiFi.setSleep(false);
        // ESP32-C3 SuperMini boards suffer RF self-interference/brownout at 19.5 dBm;
        // 8.5 dBm is the standard reliable TX power for SuperMini AP/STA operation.
        WiFi.setTxPower(WIFI_POWER_8_5dBm);

        Serial.printf("[WiFi] AP %s: SSID='%s' IP=%s\n",
                      ok ? "started" : "FAILED",
                      settings.apSsid,
                      WiFi.softAPIP().toString().c_str());

        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(DNS_PORT, "*", AP_IP);
    }

    void setupWebServer() {
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);

        auto sendIndex = [](AsyncWebServerRequest* req) {
            AsyncWebServerResponse* res = req->beginResponse(
                200, "text/html",
                reinterpret_cast<const uint8_t*>(INDEX_HTML),
                sizeof(INDEX_HTML) - 1
            );
            res->addHeader("Cache-Control", "no-store");
            req->send(res);
        };

        server.on("/", HTTP_GET, sendIndex);
        server.on("/generate_204", HTTP_GET, sendIndex);
        server.on("/gen_204", HTTP_GET, sendIndex);
        server.on("/hotspot-detect.html", HTTP_GET, sendIndex);
        server.on("/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest* req) {
            req->redirect("http://192.168.4.1/");
        });

        server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* req) {
            JsonDocument doc;
            doc["apIp"] = WiFi.softAPIP().toString();
            appendStateJson(doc["state"].to<JsonObject>());
            appendSettingsJson(doc["settings"].to<JsonObject>());

            JsonObject pins = doc["pinout"].to<JsonObject>();
            pins["servo"] = PIN_SERVO;
            pins["in1"]   = PIN_MOTOR_IN1;
            pins["in2"]   = PIN_MOTOR_IN2;
            pins["led"]   = PIN_LED;

            String out;
            serializeJson(doc, out);
            req->send(200, "application/json", out);
        });

        // HTTP control fallback: POST /api/control with JSON {"t":..,"s":..} or params t,s
        auto accumulateBody = [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
            constexpr size_t MAX_BODY = 2048;
            if (total == 0 || total > MAX_BODY) return;
            if (index == 0) {
                char* buf = static_cast<char*>(malloc(total + 1));
                if (buf) {
                    buf[total] = '\0';
                    req->_tempObject = buf;
                }
            }
            if (req->_tempObject && index + len <= total) {
                memcpy(static_cast<char*>(req->_tempObject) + index, data, len);
            }
        };

        server.on(
            "/api/control",
            HTTP_POST,
            [](AsyncWebServerRequest* req) {
                if (req->hasParam("t", true) || req->hasParam("s", true)) {
                    int16_t thr = req->hasParam("t", true) ? req->getParam("t", true)->value().toInt() : 0;
                    int16_t str = req->hasParam("s", true) ? req->getParam("s", true)->value().toInt() : 0;
                    applyControlCommand(thr, str);
                    req->send(200, "application/json", "{\"ok\":true}");
                    return;
                }
                if (req->_tempObject == nullptr) {
                    req->send(400, "application/json", "{\"error\":\"Missing or oversized body\"}");
                    return;
                }
                char* body = static_cast<char*>(req->_tempObject);
                JsonDocument doc;
                DeserializationError err = deserializeJson(doc, body);
                free(body);
                req->_tempObject = nullptr;
                if (err) {
                    req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                    return;
                }
                applyControlCommand(doc["t"] | 0, doc["s"] | 0);
                req->send(200, "application/json", "{\"ok\":true}");
            },
            nullptr,
            accumulateBody
        );

        // Update settings and persist immediately to NVS flash
        server.on(
            "/api/settings",
            HTTP_POST,
            [](AsyncWebServerRequest* req) {
                if (req->_tempObject == nullptr) {
                    req->send(400, "application/json", "{\"error\":\"Missing or oversized body\"}");
                    return;
                }
                char* body = static_cast<char*>(req->_tempObject);
                JsonDocument doc;
                DeserializationError err = deserializeJson(doc, body);
                free(body);
                req->_tempObject = nullptr;

                if (err) {
                    req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                    return;
                }

                if (doc["invertSteering"].is<bool>()) {
                    settings.invertSteering = doc["invertSteering"].as<bool>();
                }
                if (doc["invertMotor"].is<bool>()) {
                    settings.invertMotor = doc["invertMotor"].as<bool>();
                }
                if (doc["motorMinSpeed"].is<int>()) {
                    settings.motorMinSpeed = (uint8_t)constrain(doc["motorMinSpeed"].as<int>(), 0, 100);
                }
                if (doc["motorMaxSpeed"].is<int>()) {
                    settings.motorMaxSpeed = (uint8_t)constrain(doc["motorMaxSpeed"].as<int>(), 0, 100);
                }
                if (doc["failsafeMs"].is<int>()) {
                    settings.failsafeMs = (uint16_t)constrain(doc["failsafeMs"].as<int>(), 100, 5000);
                }
                if (doc["apSsid"].is<const char*>()) {
                    const char* ssid = doc["apSsid"].as<const char*>();
                    if (ssid && strlen(ssid) > 0) {
                        strlcpy(settings.apSsid, ssid, sizeof(settings.apSsid));
                    }
                }
                if (doc["apPass"].is<const char*>()) {
                    const char* pass = doc["apPass"].as<const char*>();
                    strlcpy(settings.apPass, pass ? pass : "", sizeof(settings.apPass));
                }

                Storage::sanitizeSettings(settings);
                Storage::saveSettings(settings);
                Hardware::applyOutputs(settings, state);
                Hardware::flashLedConfirm();

                JsonDocument resp;
                resp["ok"] = true;
                appendSettingsJson(resp["settings"].to<JsonObject>());
                String out;
                serializeJson(resp, out);
                req->send(200, "application/json", out);
            },
            nullptr,
            accumulateBody
        );

        server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest* req) {
            Storage::resetSettings(settings);
            Hardware::applyOutputs(settings, state);
            Hardware::flashLedConfirm();

            JsonDocument resp;
            resp["ok"] = true;
            appendSettingsJson(resp["settings"].to<JsonObject>());
            String out;
            serializeJson(resp, out);
            req->send(200, "application/json", out);
        });

        server.on("/api/reboot", HTTP_POST, [](AsyncWebServerRequest* req) {
            req->send(200, "application/json", "{\"ok\":true,\"rebooting\":true}");
            pendingReboot = true;
            rebootAtMs = millis() + 300;
        });

        // Captive portal redirect for any unknown path
        server.onNotFound([](AsyncWebServerRequest* req) {
            req->redirect("http://192.168.4.1/");
        });

        server.begin();
        Serial.println("[HTTP] Web server started on port 80");
    }
}

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== ESP32-C3 SuperMini RC Car Booting ===");

    Storage::loadSettings(settings);
    Serial.printf("[NVS] invSteer=%d invMotor=%d minSpd=%u%% maxSpd=%u%% fs=%ums ssid='%s'\n",
                  settings.invertSteering, settings.invertMotor,
                  settings.motorMinSpeed, settings.motorMaxSpeed,
                  settings.failsafeMs, settings.apSsid);

    state.throttleInput  = 0;
    state.steerInput     = 0;
    state.servoAngle     = SERVO_CENTER_DEG;
    state.failsafeActive = true;
    state.lastControlMs  = millis();

    Hardware::init(settings);
    Hardware::emergencyStop(settings, state);

    setupWiFiAP();
    setupWebServer();
}

void loop() {
    dnsServer.processNextRequest();
    ws.cleanupClients();

    uint32_t now = millis();

    // Failsafe: if no input arrives within failsafeMs, fall back to 0 throttle & center steering
    if (!state.failsafeActive && (now - state.lastControlMs) > settings.failsafeMs) {
        Hardware::emergencyStop(settings, state);
        state.failsafeActive = true;
    }

    if (now - lastTelMs >= 100) {
        lastTelMs = now;
        broadcastTelemetry();
    }

    Hardware::updateStatusLed(state, WiFi.softAPgetStationNum());

    if (pendingReboot && now >= rebootAtMs) {
        Hardware::emergencyStop(settings, state);
        delay(50);
        ESP.restart();
    }
}
