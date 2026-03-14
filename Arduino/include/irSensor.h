#pragma once
#include "Arduino.h"

#define RightIRPin 11
#define LeftIRPin 10

void setupIrSensors();
void IrSensorLoop();
int getDataRight();
int getDataLeft();
void setDisableIr(bool value);
