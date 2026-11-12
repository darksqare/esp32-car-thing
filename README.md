# ESP32-C3 SuperMini RC Car Controller

Wi-Fi Access Point & real-time WebSocket web controller for an ESP32-C3 SuperMini RC car with a steering servo (strictly clamped to **50°–130°**) and a **2-pin PWM/PWM generic H-bridge** DC motor driver.

## Features
- **Wi-Fi AP + Captive Portal**: Broadcasts `ESP32-RC-Car` (default password `12345678`) at `http://192.168.4.1/`.
- **WASD + Touch + Analog Sliders**:
  - Keyboard: `W` / `S` (throttle), `A` / `D` (steering), `Space` (emergency stop). Arrow keys are also supported.
  - On-screen multi-touch WASD buttons for phones/tablets.
  - Analog throttle & steering sliders (auto-return to `0` / `90°` on release by default).
- **Steering Servo Limits (`50°–130°`)**: Center is `90°`, clamped in hardware output to `[50, 130]` degrees.
- **2-Pin PWM/PWM H-Bridge Control**:
  - Forward: `IN1 = PWM`, `IN2 = 0`
  - Reverse: `IN1 = 0`, `IN2 = PWM`
  - Stop (`0` input): `IN1 = 0`, `IN2 = 0`
- **Motor Min/Max Speed Scaling**:
  - `0` input **always** outputs `0` PWM (motor stopped).
  - Non-zero throttle (`1%..100%`) scales linearly between `motorMinSpeed` and `motorMaxSpeed`.
- **Direction Invert Toggles**: Independent toggles to invert steering and motor polarity.
- **Flash Persistence (NVS)**: All settings survive reboots via ESP32 `Preferences`.
- **Failsafe Watchdog**: Automatically falls back to `0` throttle and `90°` steering if no control heartbeat arrives within `failsafeMs` (default `500 ms`).

## ESP32-C3 SuperMini Pinout

| ESP32-C3 Pin | Signal | Notes |
|---|---|---|
| `GPIO 3` | Steering Servo PWM | 50 Hz RC servo signal (50°–130°) |
| `GPIO 4` | H-Bridge `IN1` (PWM) | Forward speed PWM (`0` when reversing/stopped) |
| `GPIO 5` | H-Bridge `IN2` (PWM) | Reverse speed PWM (`0` when forward/stopped) |
| `GPIO 8` | Onboard Blue LED | Status indicator (active LOW) |

> **Power Note:** Power the servo and motor from your battery/BEC (do not power motors from the ESP32-C3 3.3V pin), and connect a **common GND** between the battery, H-bridge, servo, and ESP32-C3 SuperMini.

## Building & Flashing

Using PlatformIO:
```bash
pio run
pio run -t upload
```

Compiled firmware binaries (also attached to GitHub Releases):
- `firmware.bin` — Application binary (flash at `0x10000`)
- `bootloader.bin` — Bootloader (flash at `0x0`)
- `partitions.bin` — Partition table (flash at `0x8000`)
- `esp32-c3-rc-car-merged.bin` — Single merged image (flash at `0x0`)
