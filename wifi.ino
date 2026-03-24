#include "wifi.h"
#include <Arduino.h>

// These variables live in dispenser.cpp
extern int cr1, cr2, cr3;
extern int cr1_r, cr2_r, cr3_r;
extern int effective_cr1, effective_cr2, effective_cr3;
extern bool remoto;

void setupAP(const char ssid[], const char pass[],
             int ip1, int ip2, int ip3, int ip4,
             WiFiServer &server, int &status)
{
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        while (true);
    }

    WiFi.config(IPAddress(ip1, ip2, ip3, ip4));

    Serial.print("Creating access point: ");
    Serial.println(ssid);

    status = WiFi.beginAP(ssid, pass);

    if (status != WL_AP_LISTENING) {
        Serial.println("AP creation failed");
        while (true);
    }

    delay(2000);
    server.begin();
    printWiFiStatus();
}

void wifiLoop(WiFiServer &server, int &status, int ledPin, WiFiClient &client)
{
    // Detect AP connection changes
    if (status != WiFi.status()) {
        status = WiFi.status();
        Serial.println("AP status changed");

        if (status == WL_AP_CONNECTED) {
            IPAddress ip = client.remoteIP();
            Serial.print("Client connected from: ");
            Serial.println(ip);

        } else {
            Serial.println("Client disconnected");
        }
    }

    client = server.available();

    if (client) {
        String request = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                request += c;
                if (c == '\n') break;
            }
        }

        request.trim(); // important

        // -------------------------
        //        /get endpoint
        // -------------------------
        if (request.startsWith("GET /get")) {
            Serial.println("Received /get request");
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close\n");

            client.print("{");
            client.print("\"cr1\":"); client.print(cr1); client.print(",");
            client.print("\"cr2\":"); client.print(cr2); client.print(",");
            client.print("\"cr3\":"); client.print(cr3); client.print(",");
            client.print("\"cr1_r\":"); client.print(cr1_r); client.print(",");
            client.print("\"cr2_r\":"); client.print(cr2_r); client.print(",");
            client.print("\"cr3_r\":"); client.print(cr3_r); client.print(",");
            client.print("\"effective_cr1\":"); client.print(effective_cr1); client.print(",");
            client.print("\"effective_cr2\":"); client.print(effective_cr2); client.print(",");
            client.print("\"effective_cr3\":"); client.print(effective_cr3); client.print(",");
            client.print("\"mode\":\""); client.print(remoto ? "remote" : "local"); client.print("\"");
            client.print("}");
        }

        // -------------------------
        //        /set endpoint
        // -------------------------
        else if (request.startsWith("GET /set")) {
            Serial.println("Received /set request");

            int q = request.indexOf("?");
            if (q != -1) {
                String query = request.substring(q + 1);

                // --- remote CR values ---
                if (query.indexOf("cr1_r=") != -1) {
                    cr1_r = query.substring(query.indexOf("cr1_r=") + 6).toInt();
                    Serial.println("cr1_r set to " + String(cr1_r));
                }

                if (query.indexOf("cr2_r=") != -1) {
                    cr2_r = query.substring(query.indexOf("cr2_r=") + 6).toInt();
                    Serial.println("cr2_r set to " + String(cr2_r));
                }

                if (query.indexOf("cr3_r=") != -1) {
                    cr3_r = query.substring(query.indexOf("cr3_r=") + 6).toInt();
                    Serial.println("cr3_r set to " + String(cr3_r));
                }

                // --- mode ---
                if (query.indexOf("mode=remote") != -1 && !remoto) {
                    remoto = true;
                    Serial.println("Mode set to remote");
                }

                if (query.indexOf("mode=local") != -1 && remoto) {
                    remoto = false;
                    Serial.println("Mode set to local");
                }
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close\n");
            client.print("{\"status\":\"ok\"}");
        }

        client.stop();
    }
}

void printWiFiStatus()
{
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}
