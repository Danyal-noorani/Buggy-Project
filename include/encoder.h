#include <Arduino.h>
#define LeftSensor 2

#define LatchPin 1
#define ClockPin 10
#define DataPin 13
#define ResetPin 0

#define TURN_LEFT 0
#define TURN_RIGHT 1

#define DistancePerPulse 5.1

void hallSensorsSetup();
int getLCounter();
int getRCounter();
void hallSensorsLoop();
void turnDirection(int direction, int speed, int degrees);
void moveDistance(int distance, int speed);
double getAverageCounter();
double getTotalDistance();
double getSpeed();