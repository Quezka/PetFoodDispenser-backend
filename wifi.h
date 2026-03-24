#ifndef WIFI_H
#define WIFI_H

#include <WiFiS3.h>

void setupAP(const char ssid[], const char pass[],
             int ip1, int ip2, int ip3, int ip4,
             WiFiServer &server, int &status);

void wifiLoop(WiFiServer &server, int &status, int ledPin, WiFiClient &client);

void printWiFiStatus();

#endif
