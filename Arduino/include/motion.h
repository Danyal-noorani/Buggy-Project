#pragma once
#include <Arduino.h>

// PI controller gains — increase KP for faster response, KI to reduce steady-state error
#define MOTION_KP 2.0f
#define MOTION_KI 0.3f
#define MOTION_KD 0.03f

void motionLoop();
void setTargetSpeed(float cmPerSec);           // continuous speed, no distance limit
void moveAtSpeed(float cmPerSec, int seconds); // drive distanceCm at cmPerSec then stop
bool motionBusy();                             // true while moveAtSpeed is in progress
