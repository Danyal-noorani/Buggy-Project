#include <Arduino.h>

#define LeftSensor 2 // Left wheel hall-effect sensor (interrupt pin)

#define LatchPin 1  // HIGH→LOW latches counter into shift register
#define ClockPin 10 // Shifts one bit out per pulse
#define DataPin 13  // Serial data from shift register
#define ResetPin 0  // Active-HIGH clears IC counter

#define TURN_LEFT 0
#define TURN_RIGHT 1

#define WHEEL_CIRCUMFERENCE_CM 20.4f
#define LEFT_PULSES_PER_REV 8  // Interrupt CHANGE (both edges)
#define RIGHT_PULSES_PER_REV 4 // Shift-register IC

#define DistancePerPulse (WHEEL_CIRCUMFERENCE_CM / RIGHT_PULSES_PER_REV)       // 5.1 cm
#define DistancePerPulseHighRes (WHEEL_CIRCUMFERENCE_CM / LEFT_PULSES_PER_REV) // 2.55 cm

// Centre-to-centre wheel separation — increase to widen turns, decrease to tighten
#define WHEELBASE_CM 13.0f

enum MoveMode
{
    FORWARD,
    TURN,
    STOPMOVE,
    WAITING
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
void adjustDirection(int direction);