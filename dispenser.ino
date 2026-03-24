#include <Arduino.h>
#include "dispenser.h"

/* Tutte le tue variabili globali rimangono qui */
int motor           = 3;
int valore          = 255;
int led1            = 11;
int lettura         = 0;

int Q1_H2O          = 21000;
int Q2_H2O          = 42000;
int Q3_H2O          = 72000;
int Q4_H2O          = 120000;
int Q5_H2O          = 180000;

int QMP = 0, QP = 0, QM = 0, QMA = 0, QA = 0;

int ledbuzzer       = 11;
int tempo           = 0;
int tempoR          = 0;

int pos5 = 750, pos4 = 600, pos3 = 450, pos2 = 300, pos1 = 150;

int cr1 = 0, cr1_old = 0;
int cr2 = 0, cr2_old = 0;
int cr3 = 0, cr3_old = 0;

int cr1_r = 0, cr2_r = 0, cr3_r = 0;

int h = 0, m = 0, s = 0;

int Q_P = 0;
int Q_mot = 0;
int giro = 100;
int H2O_P = 1000;
int divisor = 0;

const int trigPin = 10;
const int echoPin = 9;

int duration = 0;
int distance = 0;
int livello = 50;
int sensore_livello = 0;

int tensioneBatterie = 0; // TODO anche questo da leggere da remoto

// Implementare allarme V Batteria bassa

bool remoto = false;

int effective_cr1, effective_cr2, effective_cr3;

/* --- PROTOTIPI DELLE FUNZIONI INTERNE --- */
void confronti_sw1(int lettura);
void confronti_sw2(int lettura);
void confronti_sw3(int lettura);

/* --- SETUP DEL DISPENSER --- */
void dispenserSetup() {
    pinMode(motor, OUTPUT);
    pinMode(led1, OUTPUT);
    pinMode(ledbuzzer, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    analogReference(AR_EXTERNAL);
    analogReadResolution(10);
}

/* --- LOOP DEL DISPENSER (TUTTA LA TUA LOGICA) --- */
void dispenserLoop() {
    effective_cr1 = remoto ? cr1_r : cr1;
    effective_cr2 = remoto ? cr2_r : cr2;
    effective_cr3 = remoto ? cr3_r : cr3;

    delay(16);
    s++;

    if (h >= tempo) {
        digitalWrite(led1, HIGH);
        analogWrite(motor, valore);

        delay(Q_P);

        analogWrite(motor, 0);
        digitalWrite(led1, LOW);

        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        duration = pulseIn(echoPin, HIGH);
        distance = (duration * 0.0343) / 2;

        Serial.print("Distance: ");
        Serial.println(distance);

        if (distance > livello) {
            for (int i = 0; i < 3; i++) {
                digitalWrite(led1, HIGH);
                delay(100);
                digitalWrite(led1, LOW);
                delay(100);
            }
            digitalWrite(led1, HIGH);
            delay(500);
            digitalWrite(led1, LOW);
        }

        delay(1000);

        h = 0;
        m = 0;
        s = 0;
    }

    if (s == 60) {
        m++;
        s = 0;

        if (m == 60) {
            h++;
            m = 0;

            if (h == 24) {
                h = 0;
            }
        }
    }

    cr1_old = cr1;
    cr2_old = cr2;
    cr3_old = cr3;

    confronti_sw1(analogRead(A0));
    confronti_sw2(analogRead(A1));
    confronti_sw3(analogRead(A2));

    /* lampeggi di feedback */
    if (cr1 != cr1_old) {
        for (int i = 0; i < cr1; i++) {
            digitalWrite(led1, HIGH); delay(100);
            digitalWrite(led1, LOW);  delay(100);
        }
    }

    if (cr2 != cr2_old) {
        for (int i = 0; i < cr2; i++) {
            digitalWrite(led1, HIGH); delay(100);
            digitalWrite(led1, LOW);  delay(100);
        }
    }

    if (cr3 != cr3_old) {
        for (int i = 0; i < cr3; i++) {
            digitalWrite(led1, HIGH); delay(100);
            digitalWrite(led1, LOW);  delay(100);
        }
    }
}

/* --- FUNZIONI DI LETTURA COMMUTATORI --- */
void confronti_sw1(int lettura) {
    if (lettura > pos5) { tempoR = 4; cr1 = 5; return; }
    if (lettura > pos4) { tempoR = 3; cr1 = 4; return; }
    if (lettura > pos3) { tempoR = 2; cr1 = 3; return; }
    if (lettura > pos2) { tempoR = 1; cr1 = 2; return; }
    if (lettura > pos1) {
        tempoR = 0;
        if (cr1 != 1) { h = m = s = 0; }
        cr1 = 1;
        return;
    }
}

void confronti_sw2(int lettura) {
    if (lettura > pos5) { tempo = 5; divisor = 2; cr2 = 5; return; }
    if (lettura > pos4) { tempo = 5; divisor = 3; cr2 = 4; return; }
    if (lettura > pos3) { tempo = 3; divisor = 4; cr2 = 3; return; }
    if (lettura > pos2) { tempo = 2; divisor = 6; cr2 = 2; return; }
    if (lettura > pos1) { tempo = 1; divisor = 8; cr2 = 1; return; }
}

void confronti_sw3(int lettura) {
    if (lettura > pos5) { QMA = 1; cr3 = 5; Q_P = Q5_H2O / divisor; return; }
    if (lettura > pos4) { QA  = 1; cr3 = 4; Q_P = Q4_H2O / divisor; return; }
    if (lettura > pos3) { QM  = 1; cr3 = 3; Q_P = Q3_H2O / divisor; return; }
    if (lettura > pos2) { QP  = 1; cr3 = 2; Q_P = Q2_H2O / divisor; return; }
    if (lettura > pos1) { QMP = 1; cr3 = 1; Q_P = Q1_H2O / divisor; return; }
}
