#include "motors.h"
#include <Arduino.h>
#include <WiFiS3.h>
#include "ultraSonic.h"
#include "irSensor.h"
#include "encoder.h"
#include "motion.h"

static const char ssid[] = "W4_BEST_GROUP";
static const char pass[] = "ardweeNo";
static int status = WL_IDLE_STATUS;
static WiFiServer server(5200);

static unsigned long previousMillisWiFi = 0;

// @brief sets up WiFi Service
void WiFiSetup()
{
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

        if (currentMillisWiFi - previousMillisWiFi > 10) // Polls web service only every 10 ms
        {
            previousMillisWiFi = currentMillisWiFi;

            if (client.connected())
            {
                // Buffer where input will be stored
                static char buffer[64];

                int len = client.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
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
                else if (strcmp(buffer, "RIGHT") == 0)
                {
                    turnDirection(1);
                }

                else if (strcmp(buffer, "LEFT") == 0)
                {
                    turnDirection(0);
                }

                else if (strcmp(buffer, "ARIGHT") == 0)
                {
                    adjustDirection(1);
                }

                else if (strcmp(buffer, "ALEFT") == 0)
                {
                    adjustDirection(0);
                }

                else if (strncmp(buffer, "MOVE:", 5) == 0)
                {
                    char *token = strtok(buffer + 5, ":");
                    if (token != NULL)
                    {
                        moveDistance(atoi(token));
                    }
                }

                // PI-controlled: MOVEAT:{speed_cm_per_sec}:{distance_cm}
                else if (strncmp(buffer, "MOVEAT:", 7) == 0)
                {
                    char *token = strtok(buffer + 7, ":");
                    if (token != NULL)
                    {
                        float speed = atof(token);
                        token = strtok(NULL, ":");
                        if (token != NULL)
                        {
                            moveAtSpeed(speed, atoi(token));
                        }
                    }
                }

                // PI-controlled continuous speed: SPEED:{cm_per_sec}  (0 to stop)
                else if (strncmp(buffer, "SPEED:", 6) == 0)
                {
                    setTargetSpeed(atof(buffer + 6));
                }

                else if (strcmp(buffer, "GET_DATA") == 0)
                {
                    /* ----------- NOT BEING USED -----------
                    Send flowing structure ->  distance:LeftIRP_State:RightIRP_State
                    client.println(String(getDistance()) + ":" + String(getDataLeft()) + ":" + String(getDataRight()));
                    */

                    client.println(String(getDistance()) + ":" + String(getSpeed()) + ":" + String(getTotalDistance()));
                }

                /* ----------- NOT BEING USED -----------

                Values come in the format "VALUES:{SPEED}:{BALANCE}:{TURN_MULTIPLIER}:{SLOW_DOWN_FACTOR}"
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

                */

                memset(buffer, 0, sizeof(buffer)); // Clears buffer for next command
            }
        }
    }
}