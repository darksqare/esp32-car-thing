#pragma once

#include "config.h"

namespace Storage {
    void loadSettings(CarSettings& settings);
    void saveSettings(const CarSettings& settings);
    void resetSettings(CarSettings& settings);
    void sanitizeSettings(CarSettings& settings);
}
