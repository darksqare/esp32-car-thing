#pragma once

#include <Arduino.h>

// ============================================================================
// ESP32-C3 SuperMini Pinout Configuration
// ============================================================================
// Carefully chosen non-strapping, non-USB, non-flash GPIOs on ESP32-C3 SuperMini:
//   GPIO 3 -> Steering Servo PWM (50 Hz)
//   GPIO 4 -> H-Bridge IN1 (PWM Forward)
//   GPIO 5 -> H-Bridge IN2 (PWM Reverse)
//   GPIO 8 -> Onboard Blue Status LED (active LOW on ESP32-C3 SuperMini)
// Avoided: GPIO 2, 9 (boot strapping), GPIO 18, 19 (USB D-/D+), GPIO 20, 21 (UART0)

constexpr uint8_t PIN_SERVO     = 3;
constexpr uint8_t PIN_MOTOR_IN1 = 4;
constexpr uint8_t PIN_MOTOR_IN2 = 5;
constexpr uint8_t PIN_LED       = 8;

// ============================================================================
// Steering Servo Limits & PWM Configuration
// ============================================================================
constexpr int16_t SERVO_MIN_DEG    = 50;
constexpr int16_t SERVO_MAX_DEG    = 130;
constexpr int16_t SERVO_CENTER_DEG = 90;

// Standard 50 Hz servo pulse width mapping across 0..180 deg
constexpr uint32_t SERVO_FREQ_HZ       = 50;
constexpr uint8_t  SERVO_LEDC_RES_BITS = 14;   // 16384 ticks per 20,000 us period
constexpr uint32_t SERVO_PERIOD_US     = 20000;
constexpr uint32_t SERVO_MIN_PULSE_US  = 500;  // 0 deg
constexpr uint32_t SERVO_MAX_PULSE_US  = 2500; // 180 deg

// ============================================================================
// 2-Pin PWM/PWM Generic H-Bridge Motor Configuration
// ============================================================================
constexpr uint32_t MOTOR_FREQ_HZ       = 1000; // 1 kHz works cleanly across 2-pin H-bridges
constexpr uint8_t  MOTOR_LEDC_RES_BITS = 8;    // 0..255 duty

// LEDC channels (used on Arduino-ESP32 v2.x; v3.x manages channels automatically)
constexpr uint8_t LEDC_CH_SERVO = 0;
constexpr uint8_t LEDC_CH_IN1   = 1;
constexpr uint8_t LEDC_CH_IN2   = 2;

// ============================================================================
// Persistent Settings (stored in ESP32 NVS Flash via Preferences)
// ============================================================================
struct CarSettings {
    bool     invertSteering;
    bool     invertMotor;
    uint8_t  motorMinSpeed;  // 0..100 % (applied only when throttle != 0)
    uint8_t  motorMaxSpeed;  // 0..100 %
    uint16_t failsafeMs;     // 100..5000 ms (falls back to 0 throttle & 90 deg if no input)
    char     apSsid[33];     // Wi-Fi AP SSID
    char     apPass[65];     // Wi-Fi AP Password (empty = open, >=8 chars = WPA2)
};

// ============================================================================
// Runtime Drive State
// ============================================================================
struct CarState {
    int8_t   throttleInput;  // -100..100 requested
    int8_t   steerInput;     // -100..100 requested
    uint8_t  motorPwm;       // 0..255 actual PWM output
    int8_t   motorDir;       // -1 = reverse, 0 = stopped, +1 = forward (after invert)
    int16_t  servoAngle;     // 50..130 actual degrees (after invert & clamp)
    bool     failsafeActive; // true when timed out waiting for input
    uint32_t lastControlMs;  // millis() of last received control command
};

inline CarSettings defaultSettings() {
    CarSettings s{};
    s.invertSteering = false;
    s.invertMotor    = false;
    s.motorMinSpeed  = 25;
    s.motorMaxSpeed  = 100;
    s.failsafeMs     = 500;
    strlcpy(s.apSsid, "ESP32-RC-Car", sizeof(s.apSsid));
    strlcpy(s.apPass, "12345678", sizeof(s.apPass));
    return s;
}
