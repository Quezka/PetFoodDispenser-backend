#include "wifi.h"
#include "dispenser.h"
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiServer server(80);
WiFiClient client = server.available();

int status = WL_IDLE_STATUS;

void setup() {
    Serial.begin(9600);
    while (!Serial);

    pinMode(LED_BUILTIN, OUTPUT);

    setupAP(ssid, pass, 192, 168, 4, 1, server, status);
    Serial.println("AP ready. Web server running.");

    dispenserSetup();
}

void loop() {
    wifiLoop(server, status, LED_BUILTIN, client);
    dispenserLoop();
}
