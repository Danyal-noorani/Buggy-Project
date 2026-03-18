#pragma once
#include "Arduino.h"

#define TRIG 12
#define ECHO 11

void ultraSonicSetup();
void ultraSonicLoop();
int getDistance();
void setLastValue(bool value);
bool getStop();