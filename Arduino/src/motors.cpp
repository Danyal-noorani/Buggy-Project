
#include <Arduino.h>
#include "motors.h"

static MotorMoveMode mode = STOP;
static int lastMode = 0;
static unsigned long previousMillisMotor = 0;
static uint8_t maxSpeed = 80;
static float turnMultiplier = 0.8f;
static float slowDownFactor = 0.55f;
static float motorBalance = 1.1f;
static int prevRightMotor = 0;
static int prevLeftMotor = 0;

static bool newUpdate = false;

static float turnRightFactor = 1.0f;
static float turnLeftFactor = 1.0f;

// @brief Sets up motor pins and enables forward direction
void setupMotors()
{
    pinMode(RightMotorPWM, OUTPUT);
    pinMode(LeftMotorPWM, OUTPUT);
    pinMode(RightMotorP1, OUTPUT);
    pinMode(RightMotorP2, OUTPUT);
    pinMode(LeftMotorP1, OUTPUT);
    pinMode(LeftMotorP2, OUTPUT);

    enableMotors();
    setMotors(0, 0);
}

void motorLoop()
{
    /*
     * Checks if the mode has changed or if any values
     * have been updated from processing, only then
     * update motors ( reduces random jitter )
     */
    if (lastMode != mode || newUpdate)
    {
        lastMode = mode;
        newUpdate = false;
        switch (mode)
        {
        case 0:
            // setMotors(maxSpeed, maxSpeed);
            turnLeftFactor = turnRightFactor = 1.0f;
            break;
        case 1:
            turnLeft();
            break;
        case 2:
            turnRight();
            break;
        case 3:
            stopMotors();
            break;
        default:
            break;
        }
    }
}

// @brief Enables motors (using pins)
void enableMotors()
{
    digitalWrite(RightMotorP1, HIGH);
    digitalWrite(RightMotorP2, LOW);
    digitalWrite(LeftMotorP1, HIGH);
    digitalWrite(LeftMotorP2, LOW);
}

// @brief disables motors (using pins)
void disableMotors()
{
    digitalWrite(RightMotorP1, LOW);
    digitalWrite(RightMotorP2, LOW);
    digitalWrite(LeftMotorP1, LOW);
    digitalWrite(LeftMotorP2, LOW);
}

// @brief stops motors (using PWM)
void stopMotors()
{
    analogWrite(RightMotorPWM, 0);
    analogWrite(LeftMotorPWM, 0);
}

void setMotorBackward(int motor) // motor = 1 RIght Motor Else Left Motor
{
    if (motor == 1)
    {
        digitalWrite(RightMotorP1, LOW);
        digitalWrite(RightMotorP2, HIGH);
    }
    else
    {
        digitalWrite(LeftMotorP1, LOW);
        digitalWrite(LeftMotorP2, HIGH);
    }
}

void setMotorForward(int motor)
{
    if (motor == 1)
    {
        digitalWrite(RightMotorP1, HIGH);
        digitalWrite(RightMotorP2, LOW);
    }
    else
    {
        digitalWrite(LeftMotorP1, HIGH);
        digitalWrite(LeftMotorP2, LOW);
    }
}

// @brief sets motors (using PWN)
void setMotors(int speedLeft, int speedRight)

{

    speedRight = constrain(speedRight, 0, 100);
    speedLeft = constrain(speedLeft, 0, 100);

    prevLeftMotor = speedLeft;
    prevRightMotor = speedRight;

    // scales speed values between 0 and 255 and applies motor balance
    // analogWrite(RightMotorPWM, 2.55f * motorBalanceRight * speedRight);
    analogWrite(RightMotorPWM, 2.55f * motorBalance * speedRight * turnRightFactor);

    analogWrite(LeftMotorPWM, 2.55f * speedLeft * turnLeftFactor);
}

// @brief reduces overall speed by slowDownFactor and reduces speed of right motor by another factor of turnMultiplier
void turnRight()
{
    turnRightFactor = turnMultiplier * slowDownFactor;
}

// @brief reduces overall speed by slowDownFactor and reduces speed of left motor by another factor of turnMultiplier
void turnLeft()
{
    turnLeftFactor = turnMultiplier * slowDownFactor;
}

// @brief sets movement Mode
void setMode(MotorMoveMode newMode)
{
    mode = newMode;
}

// UI Functions

// @brief sets max speed using processing
// @param newMaxSpeed New speed
void setMaxSpeed(int newMaxSpeed)
{
    // Serial.print("Max Speed: ");
    // Serial.println(newMaxSpeed);
    maxSpeed = constrain(newMaxSpeed, 0, 100);
    newUpdate = true;
}

// @brief sets turning multiplier using processing
// @param newTurnMultiplier New turn multiplier
void setTurnMultiplier(float newTurnMultiplier)
{
    // Serial.print("Turn Multiplier: ");
    // Serial.println(newTurnMultiplier);
    turnMultiplier = constrain(newTurnMultiplier, 0, 1);
    newUpdate = true;
}

// @brief sets motor balance using processing
// @param newMaxSpeed New motor balance
void setMotorBalance(float newMotorBalanceValue)
{

    // ** Might Not work **
    newMotorBalanceValue = constrain(newMotorBalanceValue, 0.8f, 1.2f);
    // Low-pass filter to smooth out noisy encoder readings
    motorBalance = motorBalance * 0.6f + newMotorBalanceValue * 0.4f;
    // Serial.print("Motor Balance: ");
    // Serial.println(motorBalance);
}

// @brief sets slow down factor using processing
// @param newMaxSpeed New slow down factor
void setSlowDownFactor(float newSlowDownFactor)
{
    Serial.print("Slow Down Factor: ");
    Serial.println(newSlowDownFactor);
    slowDownFactor = constrain(newSlowDownFactor, 0, 1);
    newUpdate = true;
}

void editMotorsSpeed(int offsetLeft, int offsetRight)
{
    setMotors(prevLeftMotor + offsetLeft, prevRightMotor + offsetRight);
}

void brake()
{
    setMotorBackward(1);
    setMotorBackward(0);
    setMotors(15, 21);
    delay(200);
    stopMotors();
    setMotorForward(1);
    setMotorForward(0);
}
