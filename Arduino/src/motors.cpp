
#include <Arduino.h>
#include "motors.h"

static MotorMoveMode mode = STOP;
static int lastMode = 0;
static unsigned long previousMillisMotor = 0;
static uint8_t maxSpeed = 80;
static float turnMultiplier = 0.17f;
static float slowDownFactor = 0.55f;
static float motorBalance = 1.1f;

static bool newUpdate = false;

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
            setMotors(maxSpeed, maxSpeed);
            break;
        case 1:
            turnRight();
            break;
        case 2:
            turnLeft();
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
    // constrains input values between -100 and 100
    speedRight = constrain(speedRight, 0, 100);
    speedLeft = constrain(speedLeft, 0, 100);

    // scales speed values between 0 and 255 and applies motor balance
    // analogWrite(RightMotorPWM, 2.55f * motorBalanceRight * speedRight);
    analogWrite(RightMotorPWM, 2.55f * motorBalance * speedRight);

    analogWrite(LeftMotorPWM, 2.55f * speedLeft);
}

// @brief reduces overall speed by slowDownFactor and reduces speed of right motor by another factor of turnMultiplier
void turnRight()
{
    setMotors(maxSpeed * slowDownFactor, maxSpeed * turnMultiplier * slowDownFactor);
}

// @brief reduces overall speed by slowDownFactor and reduces speed of left motor by another factor of turnMultiplier
void turnLeft()
{
    setMotors(maxSpeed * turnMultiplier * slowDownFactor, maxSpeed * slowDownFactor);
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
    newMotorBalanceValue = constrain(newMotorBalanceValue, 1.1f, 1.2f);
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
