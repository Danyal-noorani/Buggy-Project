# W4 Robot — Project Context for Claude

## Platform
- Arduino (PlatformIO), target board uses WiFiS3 (Arduino UNO R4 WiFi)
- Processing sketches in `../Processing/` (ArdweeNoCode, SilverChallenge)

## Module Overview

| File | Purpose |
|------|---------|
| `src/main.cpp` | Entry point — setup() / loop() |
| `src/motors.cpp` | PWM motor control, balance, brake |
| `src/encoder.cpp` | Hall-effect encoders (left = interrupt, right = shift-register IC), speed, distance, turns |
| `src/motion.cpp` | PI speed controller — `motionLoop()`, `setTargetSpeed()`, `moveAtSpeed()` |
| `src/ultraSonic.cpp` | HC-SR04 obstacle detection, `getStop()` flag |
| `src/irSensor.cpp` | IR sensor (currently commented out in main) |
| `src/wifi.cpp` | WiFi server for remote control |

## Key Constants (encoder.h)
- Wheel circumference: 20.4 cm
- Left encoder: 8 pulses/rev (interrupt CHANGE)
- Right encoder: 4 pulses/rev (shift-register IC)
- `DistancePerPulse` = 5.1 cm (right / low-res)
- `DistancePerPulseHighRes` = 2.55 cm (left / high-res)
- Wheelbase: 16.2 cm

## motion.cpp — PI Controller
- Gains: `MOTION_KP 2.0`, `MOTION_KI 0.4`
- `setTargetSpeed(float cmPerSec)` — continuous speed
- `moveAtSpeed(float cmPerSec, int distanceCm)` — drive fixed distance then stop
- `motionBusy()` — true while moveAtSpeed in progress
- Ultrasonic override: stops motors if `getStop()` is true
- **Not yet wired into main.cpp loop** — `motionLoop()` needs to be called from `loop()`

## Current State (as of 2026-03-19)
- `motion.cpp` / `motion.h` are new untracked files (PI controller just written)
- `main.cpp` includes `motion.h` but does NOT yet call `motionLoop()` in `loop()`
- `hallSensorsLoop()` is still being called — note: `motionLoop` and `hallSensorsLoop` FORWARD/TURN modes conflict; should not run simultaneously
- IR sensor is commented out

## Work In Progress
- Integrating `motionLoop()` into `main.cpp`
- PI gain tuning may be needed once tested on hardware
