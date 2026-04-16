#include <Arduino.h>
#include "dispenser.h"
#include "wifi.h"   // necessario per sendSSEUpdate()

/* --- VARIABILI GLOBALI --- */

// Attuatori
int motorPin        = 3;
int motorPower      = 255;
int statusLedPin    = 11;
int buzzerLedPin    = 11;

// Sensore ultrasuoni
const int trigPin   = 10;
const int echoPin   = 9;
int echoDuration    = 0;
int waterDistance   = 0;
int waterLevelLimit = 50;

// Timer interno
int timer_hours     = 0;
int timer_minutes   = 0;
int timer_seconds   = 0;

// Quantità acqua (ms di attivazione motore)
int qty_H2O_1       = 21000;
int qty_H2O_2       = 42000;
int qty_H2O_3       = 72000;
int qty_H2O_4       = 120000;
int qty_H2O_5       = 180000;

// Variabili derivate dai commutatori
int freq_hoursSelected = 0;   // da commutatore 2
int freq_divisor       = 0;   // da commutatore 2
int qty_msToDispense   = 0;   // da commutatore 3

// Posizioni commutatori (soglie ADC)
int adcPos1 = 150;
int adcPos2 = 300;
int adcPos3 = 450;
int adcPos4 = 600;
int adcPos5 = 750;

/* --- COMMUTATORI FISICI --- */

// Commutatore 1 → Tempo
int timeSelector_value = 0;
int timeSelector_prev  = 0;

// Commutatore 2 → Frequenza
int freqSelector_value = 0;
int freqSelector_prev  = 0;

// Commutatore 3 → Quantità
int qtySelector_value  = 0;
int qtySelector_prev   = 0;

/* --- COMMUTATORI REMOTI --- */
int timeSelector_remote = 0;
int freqSelector_remote = 0;
int qtySelector_remote  = 0;

bool remoteMode = false;

// Valori effettivi (fisici o remoti)
int timeSelector_effective;
int freqSelector_effective;
int qtySelector_effective;

// Batteria
int batteryVoltage = 0;

/* --- PROTOTIPI FUNZIONI --- */
void readTimeSelector(int adc);
void readFrequencySelector(int adc);
void readQuantitySelector(int adc);

/* --- SETUP --- */
void dispenserSetup() {
    pinMode(motorPin, OUTPUT);
    pinMode(statusLedPin, OUTPUT);
    pinMode(buzzerLedPin, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    analogReference(AR_EXTERNAL);
    analogReadResolution(10);
}

/* --- LOOP PRINCIPALE --- */
void dispenserLoop() {

    // Se remoto → usa valori remoti
    timeSelector_effective = remoteMode ? timeSelector_remote : timeSelector_value;
    freqSelector_effective = remoteMode ? freqSelector_remote : freqSelector_value;
    qtySelector_effective  = remoteMode ? qtySelector_remote  : qtySelector_value;

    delay(16);
    timer_seconds++;

    /* --- CICLO DI EROGAZIONE --- */
    if (timer_hours >= freq_hoursSelected) {

        digitalWrite(statusLedPin, HIGH);
        analogWrite(motorPin, motorPower);

        delay(qty_msToDispense);

        analogWrite(motorPin, 0);
        digitalWrite(statusLedPin, LOW);

        // Misura livello acqua
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        echoDuration = pulseIn(echoPin, HIGH);
        waterDistance = (echoDuration * 0.0343) / 2;

        Serial.print("Distance: ");
        Serial.println(waterDistance);

        if (waterDistance > waterLevelLimit) {
            for (int i = 0; i < 3; i++) {
                digitalWrite(statusLedPin, HIGH); delay(100);
                digitalWrite(statusLedPin, LOW);  delay(100);
            }
            digitalWrite(statusLedPin, HIGH); delay(500);
            digitalWrite(statusLedPin, LOW);
        }

        delay(1000);

        timer_hours = 0;
        timer_minutes = 0;
        timer_seconds = 0;
    }

    /* --- TIMER INTERNO --- */
    if (timer_seconds == 60) {
        timer_minutes++;
        timer_seconds = 0;

        if (timer_minutes == 60) {
            timer_hours++;
            timer_minutes = 0;

            if (timer_hours == 24) {
                timer_hours = 0;
            }
        }
    }

    /* --- SALVA VALORI PRECEDENTI --- */
    timeSelector_prev = timeSelector_value;
    freqSelector_prev = freqSelector_value;
    qtySelector_prev  = qtySelector_value;

    /* --- LEGGI COMMUTATORI --- */
    readTimeSelector(analogRead(A0));
    readFrequencySelector(analogRead(A1));
    readQuantitySelector(analogRead(A2));

    /* --- HOOK SSE --- */
    if (timeSelector_value != timeSelector_prev ||
        freqSelector_value != freqSelector_prev ||
        qtySelector_value  != qtySelector_prev) {
        sendSSEUpdate();
    }

    /* --- FEEDBACK LED --- */
    if (timeSelector_value != timeSelector_prev) {
        for (int i = 0; i < timeSelector_value; i++) {
            digitalWrite(statusLedPin, HIGH); delay(100);
            digitalWrite(statusLedPin, LOW);  delay(100);
        }
    }

    if (freqSelector_value != freqSelector_prev) {
        for (int i = 0; i < freqSelector_value; i++) {
            digitalWrite(statusLedPin, HIGH); delay(100);
            digitalWrite(statusLedPin, LOW);  delay(100);
        }
    }

    if (qtySelector_value != qtySelector_prev) {
        for (int i = 0; i < qtySelector_value; i++) {
            digitalWrite(statusLedPin, HIGH); delay(100);
            digitalWrite(statusLedPin, LOW);  delay(100);
        }
    }
}

/* --- LETTURA COMMUTATORE 1 (TEMPO) --- */
void readTimeSelector(int adc) {
    if (adc > adcPos5) { timeSelector_value = 5; return; }
    if (adc > adcPos4) { timeSelector_value = 4; return; }
    if (adc > adcPos3) { timeSelector_value = 3; return; }
    if (adc > adcPos2) { timeSelector_value = 2; return; }
    if (adc > adcPos1) {
        if (timeSelector_value != 1) {
            timer_hours = timer_minutes = timer_seconds = 0;
        }
        timeSelector_value = 1;
        return;
    }
}

/* --- LETTURA COMMUTATORE 2 (FREQUENZA) --- */
void readFrequencySelector(int adc) {
    if (adc > adcPos5) { freq_hoursSelected = 5; freq_divisor = 2; freqSelector_value = 5; return; }
    if (adc > adcPos4) { freq_hoursSelected = 5; freq_divisor = 3; freqSelector_value = 4; return; }
    if (adc > adcPos3) { freq_hoursSelected = 3; freq_divisor = 4; freqSelector_value = 3; return; }
    if (adc > adcPos2) { freq_hoursSelected = 2; freq_divisor = 6; freqSelector_value = 2; return; }
    if (adc > adcPos1) { freq_hoursSelected = 1; freq_divisor = 8; freqSelector_value = 1; return; }
}

/* --- LETTURA COMMUTATORE 3 (QUANTITÀ) --- */
void readQuantitySelector(int adc) {
    if (adc > adcPos5) { qtySelector_value = 5; qty_msToDispense = qty_H2O_5 / freq_divisor; return; }
    if (adc > adcPos4) { qtySelector_value = 4; qty_msToDispense = qty_H2O_4 / freq_divisor; return; }
    if (adc > adcPos3) { qtySelector_value = 3; qty_msToDispense = qty_H2O_3 / freq_divisor; return; }
    if (adc > adcPos2) { qtySelector_value = 2; qty_msToDispense = qty_H2O_2 / freq_divisor; return; }
    if (adc > adcPos1) { qtySelector_value = 1; qty_msToDispense = qty_H2O_1 / freq_divisor; return; }
}
