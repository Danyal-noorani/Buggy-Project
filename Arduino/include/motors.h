#pragma once
#include "Arduino.h"

#define RightMotorPWM 5
#define RightMotorP1 8
#define RightMotorP2 9

#define LeftMotorPWM 6
#define LeftMotorP1 4
#define LeftMotorP2 7

enum MotorMoveMode
{
    STRAIGHT,
    RIGHT,
    LEFT,
    STOP
};

void setupMotors();
void enableMotors();
void disableMotors();
void stopMotors();
void setMotors(int speedLeft, int speedRight);
void turnRight();
void turnLeft();
void setMode(MotorMoveMode mode);
void motorLoop();
void setMaxSpeed(int newMaxSpeed);
void setTurnMultiplier(float newTurnMultiplier);
void setMotorBalance(float newMotorBalanceValue);
void setSlowDownFactor(float newSlowDownFactor);
void setMotorForward(int motor);
void setMotorBackward(int motor);
void brake();
