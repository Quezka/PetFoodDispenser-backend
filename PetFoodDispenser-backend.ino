#include <Arduino_FreeRTOS.h>
#include "wifi.h"
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiServer server(80);
int status = WL_IDLE_STATUS;

TaskHandle_t wifiTaskHandle;

void setup() {
    Serial.begin(115200);
    while (!Serial);

    pinMode(LED_BUILTIN, OUTPUT);

    setupAP(ssid, pass, 192, 168, 4, 1, server, status);

    static WifiTaskParams wifiParams = {
        .server = &server,
        .status = &status,
        .ledPin = LED_BUILTIN
    };

    xTaskCreate(
        wifiTask,
        "WiFiTask",
        4096,
        &wifiParams,
        1,
        &wifiTaskHandle
    );

    vTaskStartScheduler();
}

void loop() {}
