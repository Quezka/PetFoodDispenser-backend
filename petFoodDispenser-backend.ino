#include "wifi.h"
#include "api.h"

WiFiServer server(80);

const char* SSID = "YOUR_SSID";
const char* PASS = "YOUR_PASSWORD";

void setup() {
    Serial.begin(115200);

    wifiInit(SSID, PASS);
}

void loop() {
    apiHandleClient();
}
