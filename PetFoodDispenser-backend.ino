/* 
  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#access-point
*/

#include "WiFiS3.h"
#include "arduino_secrets.h" 
#include "wifi.h"

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // network password
int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Serial Connected!");
  
  pinMode(led, OUTPUT);

  setupAP(ssid, pass, 192,168,4,1, server);
}


void loop() {
 wifiLoop(status, client, server, led);
}

