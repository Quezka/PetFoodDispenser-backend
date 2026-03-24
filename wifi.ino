#include "wifi.h"
#include <Arduino.h>

void setupAP(const char ssid[], const char pass[], int ip1, int ip2, int ip3, int ip4,
             WiFiServer &server, int &status)
{
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        while (true);
    }

    WiFi.config(IPAddress(ip1, ip2, ip3, ip4));

    Serial.print("Creating access point named: ");
    Serial.println(ssid);

    status = WiFi.beginAP(ssid, pass);

    if (status != WL_AP_LISTENING) {
        Serial.println("Creating access point failed");
        while (true);
    }

    delay(2000);
    server.begin();
    printWiFiStatus();
}

void wifiTask(void *pvParameters)
{
    WifiTaskParams *params = (WifiTaskParams *)pvParameters;

    WiFiServer &server = *params->server;
    int &status = *params->status;
    int led = params->ledPin;

    WiFiClient client;

    for (;;) {
        if (status != WiFi.status()) {
            status = WiFi.status();
            Serial.println("Access Point Web Server");

            if (status == WL_AP_CONNECTED)
                Serial.println("Device connected to AP");
            else
                Serial.println("Device disconnected from AP");
        }

        client = server.available();

        if (client) {
            Serial.println("new client");
            String currentLine = "";

            while (client.connected()) {
                if (client.available()) {
                    char c = client.read();
                    Serial.write(c);

                    if (c == '\n') {
                        if (currentLine.length() == 0) {
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-type:text/html");
                            client.println("Connection: close\n");

                            client.print(
                                "<!DOCTYPE HTML><html><head>"
                                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                                "<title>WiFi Pet Food Dispenser Control</title>"
                                "</head><body style=\"text-align:center;\">"
                                "<p style=\"font-size:2vw;\">Click <a href=\"/H\">here</a> to turn LED ON</p>"
                                "<p style=\"font-size:2vw;\">Click <a href=\"/L\">here</a> to turn LED OFF</p>"
                                "</body></html>"
                            );

                            break;
                        } else {
                            currentLine = "";
                        }
                    } else if (c != '\r') {
                        currentLine += c;
                    }

                    if (currentLine.endsWith("GET /H"))
                        digitalWrite(led, HIGH);

                    if (currentLine.endsWith("GET /L"))
                        digitalWrite(led, LOW);
                }
            }

            client.stop();
            Serial.println("client disconnected");
        }

        vTaskDelay(1);
    }
}

void printWiFiStatus()
{
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
}
