// PetFoodDispenser-backend.ino
// File Main che unisce tutti i moduli
#include "wifi.h"
#include "dispenser.h"
#include "arduino_secrets.h"

/*
*   IoT Pet Food Dispenser - IIS "E. Alessandrini"
*   v.1.0.2 - release
*   12/05/2026
*/

// impostiamo le variabili da passare con il nome della rete e la password
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// creiamo l'oggetto server da usare per le richieste dei Client
WiFiServer server(80);
WiFiClient client = server.available();

// inizializziamo la variabile status con lo status IDLE, cioè disponibile
int status = WL_IDLE_STATUS;

// setup
void setup() {
    // iniziamo comunicazione seriale a 9600 baud
    Serial.begin(9600);
    while (!Serial);

    // impostiamo il pin del led in output
    pinMode(LED_BUILTIN, OUTPUT);

    // chiamiamo la funzione per impostare il punto d'accesso
    setupAP(ssid, pass, 192, 168, 4, 1, server, status);
    Serial.println("AP ready. Web server running.");

    // impostiamo la logica del dispenser
    dispenserSetup();
}

// main loop
void loop() {
    // loop del WiFi
    wifiLoop(server, status, LED_BUILTIN, client);
    
    // looop del Dispenser
    dispenserLoop();
}
