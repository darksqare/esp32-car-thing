#include "storage.h"
#include <Preferences.h>

namespace {
    constexpr const char* NVS_NAMESPACE = "rccar";
    Preferences prefs;
}

namespace Storage {

void sanitizeSettings(CarSettings& s) {
    if (s.motorMinSpeed > 100) s.motorMinSpeed = 100;
    if (s.motorMaxSpeed > 100) s.motorMaxSpeed = 100;
    if (s.motorMinSpeed > s.motorMaxSpeed) {
        s.motorMinSpeed = s.motorMaxSpeed;
    }

    if (s.failsafeMs < 100)  s.failsafeMs = 100;
    if (s.failsafeMs > 5000) s.failsafeMs = 5000;

    s.apSsid[sizeof(s.apSsid) - 1] = '\0';
    s.apPass[sizeof(s.apPass) - 1] = '\0';

    if (strlen(s.apSsid) == 0) {
        strlcpy(s.apSsid, "ESP32-RC-Car", sizeof(s.apSsid));
    }

    // WPA2 requires >= 8 chars or empty for open AP
    size_t passLen = strlen(s.apPass);
    if (passLen > 0 && passLen < 8) {
        strlcpy(s.apPass, "12345678", sizeof(s.apPass));
    }
}

void loadSettings(CarSettings& settings) {
    settings = defaultSettings();

    if (!prefs.begin(NVS_NAMESPACE, true)) {
        saveSettings(settings);
        return;
    }

    settings.invertSteering = prefs.getBool("invSteer", settings.invertSteering);
    settings.invertMotor    = prefs.getBool("invMotor", settings.invertMotor);
    settings.motorMinSpeed  = prefs.getUChar("minSpd", settings.motorMinSpeed);
    settings.motorMaxSpeed  = prefs.getUChar("maxSpd", settings.motorMaxSpeed);
    settings.failsafeMs     = prefs.getUShort("failsafe", settings.failsafeMs);

    String ssid = prefs.getString("apSsid", settings.apSsid);
    String pass = prefs.getString("apPass", settings.apPass);
    strlcpy(settings.apSsid, ssid.c_str(), sizeof(settings.apSsid));
    strlcpy(settings.apPass, pass.c_str(), sizeof(settings.apPass));

    prefs.end();
    sanitizeSettings(settings);
}

void saveSettings(const CarSettings& input) {
    CarSettings s = input;
    sanitizeSettings(s);

    if (!prefs.begin(NVS_NAMESPACE, false)) {
        return;
    }

    prefs.putBool("invSteer", s.invertSteering);
    prefs.putBool("invMotor", s.invertMotor);
    prefs.putUChar("minSpd", s.motorMinSpeed);
    prefs.putUChar("maxSpd", s.motorMaxSpeed);
    prefs.putUShort("failsafe", s.failsafeMs);
    prefs.putString("apSsid", s.apSsid);
    prefs.putString("apPass", s.apPass);

    prefs.end();
}

void resetSettings(CarSettings& settings) {
    settings = defaultSettings();
    if (prefs.begin(NVS_NAMESPACE, false)) {
        prefs.clear();
        prefs.end();
    }
    saveSettings(settings);
}

} // namespace Storage
