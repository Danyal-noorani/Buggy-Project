#pragma once

#include <Arduino.h>
#include <WiFiS3.h>

extern char ssid[];
extern char pass[];

extern int status;
extern WiFiServer server;

void WiFiSetup();
void WiFiLoop();
