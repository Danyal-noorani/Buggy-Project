#pragma once
#include "Arduino.h"

#define RightIRPin 1
#define LeftIRPin 0
// left: 2, right: 3

void setupIrSensors();
void IrSensorLoop();
int getDataRight();
int getDataLeft();
void setDisableIr(bool value);
