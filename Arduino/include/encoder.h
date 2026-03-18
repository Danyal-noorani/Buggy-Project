#include <Arduino.h>
#define LeftSensor 2

#define LatchPin 1
#define ClockPin 10
#define DataPin 13
#define ResetPin 0

#define TURN_LEFT 0
#define TURN_RIGHT 1

#define DistancePerPulse 5.1

enum MoveMode
{
    FORWARD,
    TURN,
    STOPMOVE
};

void hallSensorsSetup();
int getLCounter(bool highRes = false);
int getRCounter();
void hallSensorsLoop();
void turnDirection(int direction);
void moveDistance(int distance);
double getAverageCounter();
double getTotalDistance();
double getSpeed();
