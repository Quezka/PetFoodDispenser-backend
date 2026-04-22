#include "wifi.h"
#include <Arduino.h>

// Variabili che arrivano da dispenser.ino
extern int cr1, cr2, cr3;
extern int cr1_r, cr2_r, cr3_r;
extern int effective_cr1, effective_cr2, effective_cr3;
extern bool remoto;

// --- Variabili per SSE ---
WiFiClient sseClient;
bool sseConnected = false;

// Prototipo della funzione dell'sse per ottimizzare
void sendSSEUpdate();

/*
Impostazione del Punto d'Accesso WiFi e Server, ssid = nome del punto d'accesso, pass = password, ip1,ip2,ip3,ip4 = indirizzo ip 192, 168, 4, 1
*/
void setupAP(const char ssid[], const char pass[],
             int ip1, int ip2, int ip3, int ip4,
             WiFiServer &server, int &status)
{
    // Controlla se il modulo WiFi funziona
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        while (true);
    }

    // Configura l'ip dell'Arduino nella rete
    WiFi.config(IPAddress(ip1, ip2, ip3, ip4));

    // Debug
    Serial.print("Creating access point: ");
    Serial.println(ssid);

    // 
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
    // Rileva cambi di stato AP
    if (status != WiFi.status()) {
        status = WiFi.status();
        Serial.println("AP status changed");

        if (status == WL_AP_CONNECTED) {
            IPAddress ip = client.remoteIP();
            Serial.print("Client connected from: ");
            Serial.println(ip);
        } else {
            Serial.println("Client disconnected");
            sseConnected = false;
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

        request.trim();

        // -------------------------
        //        /events endpoint
        // -------------------------
        if (request.startsWith("GET /events")) {
            Serial.println("SSE client connected");

            sseClient = client;
            sseConnected = true;

            sseClient.println("HTTP/1.1 200 OK");
            sseClient.println("Content-Type: text/event-stream");
            sseClient.println("Cache-Control: no-cache");
            sseClient.println("Connection: keep-alive");
            sseClient.println();

            return; // NON chiudere la connessione
        }

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

                if (query.indexOf("cr1_r=") != -1)
                    cr1_r = query.substring(query.indexOf("cr1_r=") + 6).toInt();

                if (query.indexOf("cr2_r=") != -1)
                    cr2_r = query.substring(query.indexOf("cr2_r=") + 6).toInt();

                if (query.indexOf("cr3_r=") != -1)
                    cr3_r = query.substring(query.indexOf("cr3_r=") + 6).toInt();

                if (query.indexOf("mode=remote") != -1)
                    remoto = true;

                if (query.indexOf("mode=local") != -1)
                    remoto = false;
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close\n");
            client.print("{\"status\":\"ok\"}");
        }

        client.stop();
    }
}

void sendSSEUpdate()
{
    if (!sseConnected || !sseClient.connected()) {
        sseConnected = false;
        return;
    }

    sseClient.print("event: knobUpdate\n");
    sseClient.print("data: {\"cr1\":");
    sseClient.print(cr1);
    sseClient.print(",\"cr2\":");
    sseClient.print(cr2);
    sseClient.print(",\"cr3\":");
    sseClient.print(cr3);
    sseClient.println("}\n");
    sseClient.println();
}

void printWiFiStatus()
{
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}
