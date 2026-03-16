#include <Arduino.h>
#include "ultraSonic.h"
#include "motors.h"
#include "irSensor.h"
#include "wifi.h"
#include "encoder.h"
void setup()
{
    Serial.begin(9600);

    ultraSonicSetup();
    WiFiSetup();
    // setupIrSensors();
    setupMotors();
    hallSensorsSetup();
}
void loop()
{
    WiFiLoop();
    ultraSonicLoop();
    // IrSensorLoop();
    // motorLoop();
    hallSensorsLoop();
}
