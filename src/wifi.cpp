#include <Arduino.h>
#include <WiFiS3.h>
#include "motors.h"
#include "ultraSonic.h"
#include "irSensor.h"
#include "encoder.h"

char ssid[] = "W4_BEST_GROUP";
char pass[] = "ardweeNo";
int status = WL_IDLE_STATUS;
WiFiServer server(5200);

unsigned long previousMillisWiFi = 0;

// @brief sets up WiFi Service
void WiFiSetup()
{
    Serial.begin(9600);
    Serial.print("Network named:");
    Serial.println(ssid);
    status = WiFi.beginAP(ssid, pass);
    Serial.println("Network␣started");
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    server.begin();
}

void WiFiLoop()
{
    WiFiClient client = server.available();
    if (client)
    {
        unsigned long currentMillisWiFi = millis();

        if (currentMillisWiFi - previousMillisWiFi > 10) // Polls web service only every 300 ms
        {
            previousMillisWiFi = currentMillisWiFi;

            if (client.connected())
            {
                // Buffer where input will be stored
                static char buffer[64];

                int len = client.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
                if (strcmp(buffer, "GET_DATA") != 0)
                {
                    Serial.println(buffer);
                }
                if (len <= 0)
                    return;

                // Null terminate required for strcmp
                buffer[len] = '\0';

                if (strcmp(buffer, "START") == 0)
                {
                    enableMotors();
                }

                else if (strcmp(buffer, "STOP") == 0)
                {
                    disableMotors();
                }

                else if (strcmp(buffer, "GET_DATA") == 0)
                {

                    // Send flowing structure ->  distance:LeftIRP_State:RightIRP_State
                    client.println(String(getDistance()) + ":" + String(getDataLeft()) + ":" + String(getDataRight()));
                }
                // Values come in the format "VALUES:{SPEED}:{BALANCE}:{TURN_MULTIPLIER}:{SLOW_DOWN_FACTOR}"
                else if (strncmp(buffer, "VALUES:", 7) == 0)
                {
                    int index = 0;

                    // Function from string.h, returns value from start pos till given delimeter ":"
                    char *token = strtok(buffer + 7, ":");

                    // Loops through each value
                    while (token != NULL)
                    {
                        switch (index)
                        {
                        case 0:
                            setMaxSpeed(atoi(token));
                            break;
                        case 1:
                            setMotorBalance(atof(token));
                            break;
                        case 2:
                            setTurnMultiplier(atof(token));
                            break;
                        case 3:
                            setSlowDownFactor(atof(token));
                            break;
                        }
                        index++;
                        token = strtok(NULL, ":");
                    }
                }
                // --------------------------------------------- UPDATES END ---------------------------------------------

                memset(buffer, 0, sizeof(buffer)); // Clears buffer for next command
            }
        }
    }
}