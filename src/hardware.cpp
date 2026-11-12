#include "hardware.h"

namespace {
    uint32_t ledConfirmUntilMs = 0;

    void pwmSetupPin(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t resBits) {
    #if ESP_ARDUINO_VERSION_MAJOR >= 3
        (void)channel;
        ledcAttach(pin, freq, resBits);
    #else
        ledcSetup(channel, freq, resBits);
        ledcAttachPin(pin, channel);
    #endif
    }

    void pwmWritePin(uint8_t pin, uint8_t channel, uint32_t duty) {
    #if ESP_ARDUINO_VERSION_MAJOR >= 3
        (void)channel;
        ledcWrite(pin, duty);
    #else
        ledcWrite(channel, duty);
    #endif
    }

    void writeServoAngle(int16_t angleDeg) {
        int16_t clamped = constrain(angleDeg, SERVO_MIN_DEG, SERVO_MAX_DEG);
        uint32_t pulseUs = SERVO_MIN_PULSE_US +
            ((uint32_t)clamped * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US)) / 180UL;
        uint32_t maxDuty = (1UL << SERVO_LEDC_RES_BITS);
        uint32_t duty = (pulseUs * maxDuty) / SERVO_PERIOD_US;
        pwmWritePin(PIN_SERVO, LEDC_CH_SERVO, duty);
    }
}

namespace Hardware {

void init(const CarSettings& settings) {
    (void)settings;
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, HIGH); // ESP32-C3 SuperMini LED is active LOW

    // Setup steering servo at 50 Hz and center to 90 deg
    pwmSetupPin(PIN_SERVO, LEDC_CH_SERVO, SERVO_FREQ_HZ, SERVO_LEDC_RES_BITS);
    writeServoAngle(SERVO_CENTER_DEG);

    // Setup 2-pin PWM/PWM H-bridge (IN1 & IN2 start at 0)
    pwmSetupPin(PIN_MOTOR_IN1, LEDC_CH_IN1, MOTOR_FREQ_HZ, MOTOR_LEDC_RES_BITS);
    pwmSetupPin(PIN_MOTOR_IN2, LEDC_CH_IN2, MOTOR_FREQ_HZ, MOTOR_LEDC_RES_BITS);
    pwmWritePin(PIN_MOTOR_IN1, LEDC_CH_IN1, 0);
    pwmWritePin(PIN_MOTOR_IN2, LEDC_CH_IN2, 0);
}

void applyOutputs(const CarSettings& settings, CarState& state) {
    // ------------------------------------------------------------------------
    // 1. Steering Servo: map -100..+100 -> 50..130 deg (center 90 deg)
    // ------------------------------------------------------------------------
    int16_t steer = constrain((int16_t)state.steerInput, -100, 100);
    if (settings.invertSteering) {
        steer = -steer;
    }
    int16_t angle = SERVO_CENTER_DEG + (steer * (SERVO_MAX_DEG - SERVO_CENTER_DEG)) / 100;
    angle = constrain(angle, SERVO_MIN_DEG, SERVO_MAX_DEG);
    state.servoAngle = angle;
    writeServoAngle(angle);

    // ------------------------------------------------------------------------
    // 2. 2-Pin PWM/PWM H-Bridge: 0 input ALWAYS falls back to 0 PWM (coast).
    //    Non-zero input maps 1..100% -> [motorMinSpeed .. motorMaxSpeed].
    // ------------------------------------------------------------------------
    int16_t thr = constrain((int16_t)state.throttleInput, -100, 100);
    if (thr == 0 || settings.motorMaxSpeed == 0) {
        state.motorDir = 0;
        state.motorPwm = 0;
        pwmWritePin(PIN_MOTOR_IN1, LEDC_CH_IN1, 0);
        pwmWritePin(PIN_MOTOR_IN2, LEDC_CH_IN2, 0);
        return;
    }

    if (settings.invertMotor) {
        thr = -thr;
    }

    int8_t dir = (thr > 0) ? 1 : -1;
    uint16_t mag = (uint16_t)abs(thr); // 1..100

    uint16_t minPct = settings.motorMinSpeed;
    uint16_t maxPct = settings.motorMaxSpeed;
    if (minPct > maxPct) minPct = maxPct;

    uint16_t minPwm = (minPct * 255UL) / 100UL;
    uint16_t maxPwm = (maxPct * 255UL) / 100UL;

    uint16_t pwm = minPwm;
    if (mag > 1) {
        pwm = minPwm + ((mag - 1UL) * (maxPwm - minPwm)) / 99UL;
    }
    if (pwm > 255) pwm = 255;

    state.motorDir = dir;
    state.motorPwm = (uint8_t)pwm;

    pwmWritePin(PIN_MOTOR_IN1, LEDC_CH_IN1, (dir > 0) ? pwm : 0);
    pwmWritePin(PIN_MOTOR_IN2, LEDC_CH_IN2, (dir < 0) ? pwm : 0);
}

void emergencyStop(const CarSettings& settings, CarState& state) {
    state.throttleInput = 0;
    state.steerInput = 0;
    applyOutputs(settings, state);
}

void flashLedConfirm() {
    ledConfirmUntilMs = millis() + 250;
}

void updateStatusLed(const CarState& state, uint8_t wifiClients) {
    uint32_t now = millis();
    if (now < ledConfirmUntilMs) {
        digitalWrite(PIN_LED, ((now / 50) % 2) ? LOW : HIGH);
        return;
    }

    if (state.motorPwm > 0) {
        digitalWrite(PIN_LED, LOW);
    } else if (wifiClients > 0) {
        digitalWrite(PIN_LED, ((now % 1500) < 120) ? LOW : HIGH);
    } else {
        digitalWrite(PIN_LED, ((now % 1000) < 500) ? LOW : HIGH);
    }
}

} // namespace Hardware
