#include "wifi.h"
#include <Arduino.h>

// Variabili che arrivano da dispenser.ino
extern int cr1, cr2, cr3;
extern int cr1_r, cr2_r, cr3_r;
extern int effective_cr1, effective_cr2, effective_cr3;
extern bool remoto;

// Nuove istruzioni
extern bool test, tipo_dispenser;
extern int volume_min, prolunghe_serbatoio;
extern bool eroga_ora;

// --- Variabili per SSE ---
WiFiClient sseClient;
bool sseConnected = false;

// Prototipo della Funzione per l'SSE per ottimizzare
void sendSSEUpdate();

String getParam(const String &query, const String &key) {
    int start = query.indexOf(key + "=");
    if (start == -1) return "";
    start += key.length() + 1;

    int end = query.indexOf("&", start);
    if (end == -1) end = query.length();

    return query.substring(start, end);
}


// Funzione che imposta il punto d'accesso e il server
/*
ssid - nome del punto d'accesso (rete wifi), 
pass - password della rete, 
ip1,ip2,ip3,ip4 - indirizzo IP dell'Arduino nella rete, 
&server - puntatore all'oggetto server, -|
                                         |--> entrambi dichiarati in PetFoodDispenser-backend.ino
&status - puntatore all'oggetto status  -|
*/ 
void setupAP(const char ssid[], const char pass[],
             int ip1, int ip2, int ip3, int ip4,
             WiFiServer &server, int &status)
{
    // Controllo della comunicazione con il modulo WiFi
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        while (true);
    }

    // Impostazione dell'indirizzo IP 
    WiFi.config(IPAddress(ip1, ip2, ip3, ip4));

    // Debug
    Serial.print("Creating access point: ");
    Serial.println(ssid);

    // Impostazione dello stato del punto d'accessp
    status = WiFi.beginAP(ssid, pass);

    // Fine se il punto d'accesso non si è creato
    if (status != WL_AP_LISTENING) {
        Serial.println("AP creation failed");
        while (true);
    }

    delay(2000);
    server.begin();
    printWiFiStatus();
}

// Loop principale della logica di rete,
/*
&server = puntatore all'oggetto server creato in PetFoodDispenser-backend.ino,
&status = puntatore all'oggetto status creato in PetFoodDispenser-backend.ino,
ledPin  = variable integer che contiene il pin del LED del circuito,
&client = puntatore all'oggetto client creato in PetFoodDispenser-backend.ino
*/
void wifiLoop(WiFiServer &server, int &status, int ledPin, WiFiClient &client)
{
    // Rileva cambi di stato del punto d'accesso
    if (status != WiFi.status()) {
        // Se cambiato
        status = WiFi.status();
        Serial.println("AP status changed");

        if (status == WL_AP_CONNECTED) {
            // Client connesso
            IPAddress ip = client.remoteIP();
            Serial.print("Client connected from: ");
            Serial.println(ip);
        } else {
            // Client disconnesso
            Serial.println("Client disconnected");
            sseConnected = false;
        }
    }

    // Passiamo il client connesso dentro la variabile
    client = server.available();

    // Se esiste un client e ha fatto una richiesta GET
    if (client) {
        // Preleviamo la richiesta
        String request = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                request += c;
                if (c == '\n' || c == '\r') break;
            }
        }

        // Eliminiamo gli spazi dalla richiesta del client
        request.trim();
        int httpIndex = request.indexOf(" HTTP/");
        if (httpIndex != -1) {
            request = request.substring(0, httpIndex);
        }


        // -------------------------
        //        Se la richiesta è di tipo /events
        // -------------------------
        if (request.startsWith("GET /events")) {
            // Voldire che il client ha connesso via SSE per avere una sessione prolungata e ricevere aggiornamenti se cambiano i commutatori rotativi
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
        //        Se la richiesta è di tipo /get
        // -------------------------
        if (request.startsWith("GET /get")) {
            // Passiamo al client una risposta JSON con le info sullo stato corrente del tutto
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
        //        Se la richiesta è di tipo /set
        // -------------------------
        else if (request.startsWith("GET /set")) {
            Serial.println("Received /set request");

            int q = request.indexOf("?");
            if (q != -1) {
                String query = request.substring(q + 1);
                Serial.println(query);

                // cr1_r
                String v = getParam(query, "cr1_r");
                if (v.length()) {
                    cr1_r = v.toInt();
                    Serial.println("cr1_r changed: " + String(cr1_r));
                }

                // cr2_r
                v = getParam(query, "cr2_r");
                if (v.length()) {
                    cr2_r = v.toInt();
                    Serial.println("cr2_r changed: " + String(cr2_r));
                }

                // cr3_r
                v = getParam(query, "cr3_r");
                if (v.length()) {
                    cr3_r = v.toInt();
                    Serial.println("cr3_r changed: " + String(cr3_r));
                }

                // volume_min
                v = getParam(query, "volume_min");
                if (v.length()) {
                    volume_min = v.toInt();
                    Serial.println("volume_min changed: " + String(volume_min));
                }

                // prolunghe_serbatoio
                v = getParam(query, "prolunghe_serbatoio");
                if (v.length()) {
                    prolunghe_serbatoio = v.toInt();
                    Serial.println("prolunghe_serbatoio changed: " + String(prolunghe_serbatoio));
                }

                // eroga_ora
                v = getParam(query, "erogate");
                if (v == "1") {
                    eroga_ora = true;
                    Serial.println("eroga_ora: true");
                }

                // test
                v = getParam(query, "test");
                if (v == "1") {
                    test = true;
                    Serial.println("test: true");
                } else if (v == "0") {
                    test = false;
                    Serial.println("test: false");
                }

                // tipo_dispenser
                v = getParam(query, "tipo_dispenser");
                if (v == "1") {
                    tipo_dispenser = true;
                    Serial.println("tipo_dispenser: true");
                } else if (v == "0") {
                    tipo_dispenser = false;
                    Serial.println("tipo_dispenser: false");
                }

                // mode
                v = getParam(query, "mode");
                if (v == "remote") {
                    remoto = true;
                    Serial.println("remoto: true");
                } else if (v == "local") {
                    remoto = false;
                    Serial.println("remoto: false");
                }
            }

            // Response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close\n");
            client.print("{\"status\":\"ok\"}");
        }


        client.stop();
    }
}

// Aggiornamento per il cliente SSE
void sendSSEUpdate(int event)
{
    // Se il cliente non è connesso, usciamo
    if (!sseConnected || !sseClient.connected()) {
        sseConnected = false;
        return;
    }

    switch (event) {
        case 1:
            sseClient.print("event: knobUpdate\n");
            sseClient.print("data: {\"cr1\":");
            sseClient.print(cr1);
            sseClient.print(",\"cr2\":");
            sseClient.print(cr2);
            sseClient.print(",\"cr3\":");
            sseClient.print(cr3);
            sseClient.println("}\n");
            sseClient.println();
            break;
        case 2:
            sseClient.print("event: erogation\n");
            sseClient.print("data: {\"is_erogating\":");
            sseClient.print(1);
            sseClient.println("}\n");
            sseClient.println();
            break;
        case 3:
            sseClient.print("event: erogation\n");
            sseClient.print("data: {\"is_erogating\":");
            sseClient.print(0);
            sseClient.println("}\n");
            sseClient.println();
            break;
    }
}

// Funzione che printa lo stato corrente del WiFi
void printWiFiStatus()
{
    // Nome punto d'Accesso
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // Indirizzo IP dell'Arduino
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}