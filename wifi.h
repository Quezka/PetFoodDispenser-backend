#ifndef WIFI_TASK_H
#define WIFI_TASK_H

#include <WiFiS3.h>

struct WifiTaskParams {
    WiFiServer *server;
    int *status;
    int ledPin;
};

void setupAP(const char ssid[], const char pass[], int ip1, int ip2, int ip3, int ip4,
             WiFiServer &server, int &status);

void wifiTask(void *pvParameters);
void printWiFiStatus();

#endif
