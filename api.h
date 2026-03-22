#pragma once
#include <WiFiS3.h>
#include "json_utils.h"
#include "sensors.h"
#include "control.h"

extern WiFiServer server;

void handleStatus(WiFiClient& client) {
    String json = buildStatusJson();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.print(json);
}

void handleControl(WiFiClient& client, const String& body) {
    applyControlCommand(body);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.print("{\"ok\":true}");
}

void apiHandleClient() {
    WiFiClient client = server.available();
    if (!client) return;

    String req = client.readStringUntil('\r');
    client.readStringUntil('\n'); // consume newline

    String body = "";
    if (req.startsWith("POST")) {
        while (client.available()) {
            body += client.readString();
        }
    }

    if (req.startsWith("GET /status")) {
        handleStatus(client);
    }
    else if (req.startsWith("POST /control")) {
        handleControl(client, body);
    }
    else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Connection: close");
        client.println();
    }

    client.stop();
}
