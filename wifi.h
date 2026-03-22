#pragma once
#include <WiFiS3.h>

extern WiFiServer server;

void wifiInit(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
    }

    server.begin();
}
