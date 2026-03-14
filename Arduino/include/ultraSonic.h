#pragma once
#include "Arduino.h"

#define TRIG 3
#define ECHO 2

void ultraSonicSetup();
void ultraSonicLoop();
int getDistance();
void setLastValue(bool value);