#include "irSensor.h"
#include "motors.h"

int LeftIRP_State = 1;
int RightIRP_State = 1;
unsigned long previousMillisIR = 0;
bool disableIR = false;

// @brief setup IR sensor pins
void setupIrSensors()
{
    pinMode(LeftIRPin, INPUT);
    pinMode(RightIRPin, INPUT);
}

void IrSensorLoop()
{
    // --------------------------------------------- UPDATES ---------------------------------------------

    unsigned long currentMillisIR = millis();
    if (currentMillisIR - previousMillisIR >= 50 && !disableIR)
    {
        previousMillisIR = currentMillisIR;

        // --------------------------------------------- UPDATES END ---------------------------------------------

        LeftIRP_State = digitalRead(LeftIRPin);
        RightIRP_State = digitalRead(RightIRPin);

        if (LeftIRP_State == LOW && RightIRP_State == LOW)
        {
            setMode(STOP);
        }

        else if (LeftIRP_State == LOW && RightIRP_State == HIGH)
        {
            setMode(LEFT);
        }

        else if (LeftIRP_State == HIGH && RightIRP_State == LOW)
        {
            setMode(RIGHT);
        }

        else if (LeftIRP_State == HIGH && RightIRP_State == HIGH)
        {
            setMode(STRAIGHT);
        }
    }
}

// @brief returns IR sensor data (using function to prevent issues with accessing variables from other files directly)
int getDataLeft()
{
    return LeftIRP_State;
}

// @brief returns IR sensor data (using function to prevent issues with accessing variables from other files directly)
int getDataRight()
{
    return RightIRP_State;
}

/*
 * @brief disables IR sensors,
 * This is to prevent IR sensors from sending motor commands when buggy
 * is stopped using UltraSonic sensor, if not done the IR sensors will restart
 * the buggy instantly
 */
void setDisableIr(bool value)
{
    disableIR = value;
    if (value)
        setMode(STOP);
}