#pragma once

#include "config.h"

namespace Hardware {
    void init(const CarSettings& settings);
    void applyOutputs(const CarSettings& settings, CarState& state);
    void emergencyStop(const CarSettings& settings, CarState& state);
    void updateStatusLed(const CarState& state, uint8_t wifiClients);
    void flashLedConfirm();
}
