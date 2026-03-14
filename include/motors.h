#pragma once
#include "Arduino.h"

// #define RightMotorPWM 11
// #define RightMotorP1 12
// #define RightMotorP2 8

// #define LeftMotorPWM 9
// #define LeftMotorP1 10
// #define LeftMotorP2 13

// #define STRAIGHT 0
// #define TURN_RIGHT 1
// #define TURN_LEFT 2
// #define STOP 3

#define RightMotorPWM 5
#define RightMotorP1 8
#define RightMotorP2 9

#define LeftMotorPWM 6
#define LeftMotorP1 4
#define LeftMotorP2 7

#define STRAIGHT 0
#define TURN_RIGHT 1
#define TURN_LEFT 2
#define STOP 3

void setupMotors();
void enableMotors();
void disableMotors();
void stopMotors();
void setMotors(int speedLeft, int speedRight);
void turnRight();
void turnLeft();
void setMode(int mode);
void motorLoop();
void setMaxSpeed(int newMaxSpeed);
void setTurnMultiplier(float newTurnMultiplier);
void setMotorBalance(float newMotorBalanceValue);
void setSlowDownFactor(float newSlowDownFactor);
void setMotorForward(int motor);
void setMotorBackward(int motor);