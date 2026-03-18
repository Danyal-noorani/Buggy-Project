#include "ultraSonic.h"
#include "irSensor.h"
#include "encoder.h"
#include "motors.h"

int distance;
unsigned long previousMillisUS = 0;
int repCount = 0;
bool stop = false;

// @brief sets up ultrasonic sensor
void ultraSonicSetup()
{
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
}

void ultraSonicLoop()
{
    unsigned long currentMillisUS = millis();

    if (currentMillisUS - previousMillisUS >= 150) // waits 150 ms to pass before reading
    {
        previousMillisUS = currentMillisUS;

        digitalWrite(TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);

        /*
         * Third argument in pulseIn is to set timeout,
         * this setting limits the sensor to see a max of 50 cm.
         * This is done as pluseIn is a blocking function and will
         * stop the loop till it gets a reading causing delay/jitter
         */
        int duration = pulseIn(ECHO, HIGH, 3000);
        if (!duration) // ie if duration == 0
        {
            repCount = 0;
            distance = 50;
            Serial.println("No Obsticale");
            setDisableIr(false);
            stop = false;
            return;
        }
        else
        {
            distance = duration * 0.034 / 2;
        }

        if (distance < 25)
        {

            /*
             * RepCount is used to check how many times its seen an obstacle
             * This is done to prevent false positives due to inconsistencies
             * in the sensor ( fixes the constant start stop bug )
             */

            repCount++;
        }
        else
        {
            repCount = 0;
        }

        if (repCount >= 3)
        {
            Serial.println("Obstacle");
            setDisableIr(true);
            stop = true;
        }
        else
        {
            Serial.println("No Obsticale");
            setDisableIr(false);
            stop = false;
        }
    }
}

// @brief returns distance using function to prevent issues with accessing variables from other files directly
int getDistance() { return distance; }

bool getStop() { return stop; }