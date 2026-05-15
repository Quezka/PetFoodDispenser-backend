// dispenser.ino

#include <Arduino.h>
#include "dispenser.h"
#include "wifi.h"   // <-- necessario per sendSSEUpdate()
#include "EEPROM.h"
// indirizzo 0 => prolunghe_serbatoio, aggiunge 25cm ?? ogni prolunga

/* Le Variabili Globali del Dispenser */
int motor           = 3;
int valore          = 255;
int led1            = 11;
int lettura         = 0;
int vbatt_low       = 6;
int Q1_H2O          = 30000; //350ml        per H2O verificati i tempi
int Q2_H2O          = 60000; //700ml
int Q3_H2O          = 103000; //1200ml
int Q4_H2O          = 172000; //2000ml
int Q5_H2O          = 257000; //3000ml

int Q1_cr          = 21000; //100g           attenzione...da verificare con prove sperimentali
int Q2_cr          = 42000; //180g
int Q3_cr          = 72000; //250g
int Q4_cr          = 120000; //400g
int Q5_cr          = 180000; //800g

int QMP = 0, QP = 0, QM = 0, QMA = 0, QA = 0;

int ledbuzzer       = 11;
int tempo           = 0;
int tempoR          = 0;
int Vbatt           = 0;  //in mV
<<<<<<< HEAD
short batteria_scarica= 0; //0= ok, 1=scarica
=======
int batteria_scarica= 0; //0= ok, 1=scarica
>>>>>>> ca0ba671bf61672d6772308876bc6ff786f16f5c
int pos5 = 750, pos4 = 600, pos3 = 450, pos2 = 300, pos1 = 150;

int cr1 = 0, cr1_old = 0;
int cr2 = 0, cr2_old = 0;
int cr3 = 0, cr3_old = 0;

int cr1_r = 0, cr2_r = 0, cr3_r = 0;

int h = 0, m = 0, s = 0, h_rimanenti=0, m_rimanenti=0;

int Q_P = 0;
int Q_mot = 0;
int giro = 100;
int H2O_P = 1000;
int divisor = 0;

const int trigPin = 10;
const int echoPin = 9;
<<<<<<< HEAD
short prolunghe_serbatoio=1; //per ogni prolunga aggiungere 12.5cm 
short prolunghe_serbatoio1=1;
=======
int prolunghe_serbatoio=1; //per ogni prolunga aggiungere 12.5cm 
>>>>>>> ca0ba671bf61672d6772308876bc6ff786f16f5c
const int Hbase=95; //mm
const int Hprolunga=125; //mm
const int superfice=28338;// mm2
int duration = 0;
int distance = 0;
<<<<<<< HEAD
short volume_min= 0; 
short volume_min1= 0;//1000 ml o cc sarebbe 1 litro
//int sensore_livello = 0;
float volume=0;
short volume_int=0;
short offset=2;  //altezza sensore livello in cm dal livello max
short tensioneBatteria = 0;
bool test = true;
bool test1 = true;
=======
int volume_min= 0; //1000 ml o cc sarebbe 1 litro
//int sensore_livello = 0;
float volume=0;
int volume_int=0;
int offset=2;  //altezza sensore livello in cm dal livello max
int tensioneBatteria = 0;
bool test = true;
>>>>>>> ca0ba671bf61672d6772308876bc6ff786f16f5c
bool remoto = false;

int effective_cr1, effective_cr2, effective_cr3;
bool reset_tempi;
bool prima_erogazione;
bool dir_motore=false;
bool allarme_livello=false;
<<<<<<< HEAD
bool tipo_dispenser=false;
bool tipo_dispenser1=false; //0 acqua o 1 crocchette
=======
bool tipo_dispenser=false; //0 acqua o 1 crocchette
>>>>>>> ca0ba671bf61672d6772308876bc6ff786f16f5c
bool eroga_ora=false;

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
    pinMode(vbatt_low, INPUT);
    pinMode(dir_motore, OUTPUT); //false => antiorario, true=> orario
    analogReference(AR_EXTERNAL); //riferimento analogico 3.3V
    analogReadResolution(10);
<<<<<<< HEAD
    EEPROM.get(0,test);
    EEPROM.get(2,tipo_dispenser);
    EEPROM.get(4,prolunghe_serbatoio);
    EEPROM.get(6,volume_min);
    Serial.print("lettura iniziale eeprom: ");
    Serial.print("remoto: ");
=======
    /*test=EEPROM.read(0);
    tipo_dispenser=EEPROM.read(1);
    prolunghe_serbatoio=EEPROM.read(2);
    volume_min=EEPROM.read(3);*/
    /*Serial.print("remoto: ");
>>>>>>> ca0ba671bf61672d6772308876bc6ff786f16f5c
    Serial.println(remoto);
    Serial.print("test: ");
    Serial.println(test);
    Serial.print("tipo_dispenser: ");
    Serial.println(tipo_dispenser);
    Serial.print("volume_min: ");
    Serial.println(volume_min);
    Serial.print("prolunghe_serbatoio: ");
<<<<<<< HEAD
    Serial.println(prolunghe_serbatoio);
=======
    Serial.println(prolunghe_serbatoio);*/
>>>>>>> ca0ba671bf61672d6772308876bc6ff786f16f5c
}

/* --- LOOP DEL DISPENSER (TUTTA LA TUA LOGICA) --- */
void dispenserLoop() {
    //verifico i dati in EEPROM per aggiornare da remoto
<<<<<<< HEAD
  if(remoto){
        EEPROM.get(0,test1);
        EEPROM.get(2,tipo_dispenser1);
        EEPROM.get(4,prolunghe_serbatoio1);
        EEPROM.get(6,volume_min1);
        if (test != test1) {EEPROM.put(0,test);}
        if (tipo_dispenser != tipo_dispenser1) {EEPROM.put(2,tipo_dispenser);}
        if (prolunghe_serbatoio != prolunghe_serbatoio1) {EEPROM.put(4,prolunghe_serbatoio);}
        if (volume_min != volume_min1) {EEPROM.put(6,volume_min);}
             }
    else {
         }
=======
  /* if(remoto){
        if (test != EEPROM.read(0)) {EEPROM.write(0,test);}
        if (tipo_dispenser != EEPROM.read(1)) {EEPROM.write(1,tipo_dispenser);}
        if (prolunghe_serbatoio != EEPROM.read(2)) {EEPROM.write(2,prolunghe_serbatoio);}
        if (volume_min != EEPROM.read(3)) {EEPROM.write(3,volume_min);}
       
            }
    else {
   


    }*/
>>>>>>> ca0ba671bf61672d6772308876bc6ff786f16f5c
    Vbatt=12000*analogRead(A3)/516;
    if (test==true)  {delay(12); }
    else {delay(1000);}
    s++;

    if (h==(tempo-1)) { h_rimanenti=0; m_rimanenti=(60-m);}
    else { h_rimanenti=(tempo-h); m_rimanenti=(60-m);}         //*****Per APP....h_rimanenti e m_rimanenti vanno letti da App per verificare il tempo prossima erogazione

       

    if ((h >= tempo) || (prima_erogazione) || (eroga_ora)) {
        sendSSEUpdate(2);
        prima_erogazione=false;
        eroga_ora=false;
        digitalWrite(led1, HIGH);
        analogWrite(motor, valore);                  //erogazione
        Serial.print("tempo erogazione in secondi: ");
        Serial.println(int(Q_P/1000));
        Serial.print("remoto: ");
        Serial.println(remoto);
        Serial.print("test: ");
        Serial.println(test);
        Serial.print("tipo_dispenser: ");
        Serial.println(tipo_dispenser);
        Serial.print("volume_min: ");
        Serial.println(volume_min);
        Serial.print("prolunghe_serbatoio: ");
        Serial.println(prolunghe_serbatoio);
        delay(Q_P);

        analogWrite(motor, 0);
        digitalWrite(led1, LOW);
                                                     
        digitalWrite(trigPin, LOW);                 //lettura Sensore ultrasuoni
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);                                        //gestione misura livello
        duration = pulseIn(echoPin, HIGH);
        distance = (((duration * 0.0343) / 2)-offset)*10;  //in mm
        Serial.print("Distance_sensore in mm: ");
        Serial.println(distance);
        if (distance > int(Hbase+prolunghe_serbatoio*Hprolunga)){ distance=Hbase+prolunghe_serbatoio*Hprolunga; Serial.print("Distance_limitata in mm: ");
        Serial.println(distance);}
        volume=(((Hbase+prolunghe_serbatoio*Hprolunga)-distance)*superfice)/1000;  //volume in CC o ml   nota...APP deve leggere Volume
        Serial.print("volume in cc o ml: ");
        Serial.println(int(volume));
        
        if (volume < volume_min) {
            for (int i = 0; i < 5; i++) {
                digitalWrite(led1, HIGH);
                delay(100);
                digitalWrite(led1, LOW);
                delay(100);
                                        }
            allarme_livello=true;
            digitalWrite(led1, HIGH);
            delay(500);
            digitalWrite(led1, LOW);
                                  }
        else {allarme_livello=false;}

        batteria_scarica=digitalRead(vbatt_low);
        if (batteria_scarica) {
            for (int i = 0; i < 20; i++) {
                digitalWrite(led1, HIGH);
                delay(50);
                digitalWrite(led1, LOW);
                delay(50);
            }
            digitalWrite(led1, HIGH);
            delay(1000);
            digitalWrite(led1, LOW);
        }
        sendSSEUpdate(3);
        delay(1000);

        if (!remoto) {
            sendSSEUpdate(1);
        }

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

    // Salva vecchi valori
    /*if (remoto==false) {
    cr1_old = cr1;
    cr2_old = cr2;
    cr3_old = cr3;
    }
    else { 
        Serial.print("cr1: ");
        Serial.println(effective_cr1);
        Serial.print("cr1_old: ");
        Serial.println(cr1_old);
        Serial.print("cr2: ");
        Serial.println(effective_cr2);
        Serial.print("cr2_old: ");
        Serial.println(cr2_old);
        Serial.print("cr3_old: ");
        Serial.print(cr3_old);
        Serial.print("cr3: ");
        Serial.println(effective_cr3);

     Serial.println("Premi un tasto per continuare...");

     while(!Serial.available());  // pausa

     Serial.read();  // svuota il buffer

     Serial.println("Riparto!");*/

    cr1_old = cr1;
    cr2_old = cr2;
    cr3_old = cr3;
    //}
     
    effective_cr1 = remoto ? cr1_r : cr1;    //aggiornamento cr1,2,3
    effective_cr2 = remoto ? cr2_r : cr2;
    effective_cr3 = remoto ? cr3_r : cr3;


    if (!remoto) {
    // Leggi commutatori
        confronti_sw1(analogRead(A0));
        confronti_sw2(analogRead(A1));
        confronti_sw3(analogRead(A2));
    }
    else {
        //aggiornamento variabili di erogazione da remoto
        switch (cr1_r)   {
            case 1:
                tempoR = 0;
                if (!reset_tempi) { 
                    h = m = s = 0; 
                    reset_tempi=true;
                    prima_erogazione=true;
                }
                break;
            case 2:
                tempoR = 1;
                reset_tempi=false;
                break;
            case 3:
                tempoR = 2;
                reset_tempi=false;
                break;
            case 4:
                tempoR = 3;
                reset_tempi=false;
                break;
            case 5:
                tempoR = 4;
                reset_tempi=false;
                break;
            default:
                tempoR = 0;
                reset_tempi=false;
                break;
                            }  
        switch (cr2_r)      {
            case 1:
                tempo = 3;
                divisor=8;
                break;
            case 2:
                tempo = 4;
                divisor=6;
                break;
            case 3:
            tempo = 6;
            divisor=4;
            break;
            case 4:
            tempo = 8;
            divisor=3;
            break;
            case 5:
            tempo = 12;
            divisor=2;
            break;
            default:
            tempo = 4;
            divisor=6;
            break;
                                } 
            switch (cr3_r)   {
            case 1:
            if (!tipo_dispenser) {Q_P = Q5_H2O / divisor; }
            else {Q_P = Q5_cr / divisor; }
            break;
            case 2:
            if (!tipo_dispenser) {Q_P = Q4_H2O / divisor; }
            else {Q_P = Q4_cr / divisor; }
            break;
            case 3:
            if (!tipo_dispenser) {Q_P = Q3_H2O / divisor; }
            else {Q_P = Q3_cr / divisor; }
            break;
            case 4:
            if (!tipo_dispenser) {Q_P = Q2_H2O / divisor; }
            else {Q_P = Q2_cr / divisor; }
            break;
            case 5:
            if (!tipo_dispenser) {Q_P = Q1_H2O / divisor; }
            else {Q_P = Q1_cr / divisor; }
            break;
            default:
            if (!tipo_dispenser) {Q_P = Q3_H2O / divisor; }
            else {Q_P = Q3_cr / divisor; }
            break;
                                } 
         } 

   

    // --- HOOK SSE: invia aggiornamento se cambia qualcosa ---
    if (((cr1 != cr1_old) || (cr2 != cr2_old) || (cr3 != cr3_old)) && (!remoto)) {
        sendSSEUpdate(1);
    }

    /* lampeggi di feedback */
    if (cr1 != cr1_old) {  //qui va messo effective_cr1 quando funzionerà bene l'app
        for (int i = 0; i <cr1; i++) {
            digitalWrite(led1, HIGH); delay(100);
            digitalWrite(led1, LOW);  delay(100);
        }
    }

    if (cr2 != cr2_old) { //qui va messo effective_cr2 quando funzionerà bene l'app
        for (int i = 0; i < cr2; i++) {
            digitalWrite(led1, HIGH); delay(100);
            digitalWrite(led1, LOW);  delay(100);
        }
    }

    if (cr3 != cr3_old) { //qui va messo effective_cr3 quando funzionerà bene l'app
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
        if (cr1 != 1) { h = m = s = 0, prima_erogazione=true;}
        cr1 = 1;
        return;
    }
}

void confronti_sw2(int lettura) {
    if (lettura > pos5) { tempo = 12; divisor = 2; cr2 = 5; return; }
    if (lettura > pos4) { tempo = 8; divisor = 3; cr2 = 4; return; }
    if (lettura > pos3) { tempo = 6; divisor = 4; cr2 = 3; return; }
    if (lettura > pos2) { tempo = 4; divisor = 6; cr2 = 2; return; }
    if (lettura > pos1) { tempo = 3; divisor = 8; cr2 = 1; return; }
}

void confronti_sw3(int lettura) {
    if (lettura > pos5) { QMA = 1; cr3 = 5; Q_P = Q5_H2O / divisor; return; }
    if (lettura > pos4) { QA  = 1; cr3 = 4; Q_P = Q4_H2O / divisor; return; }
    if (lettura > pos3) { QM  = 1; cr3 = 3; Q_P = Q3_H2O / divisor; return; }
    if (lettura > pos2) { QP  = 1; cr3 = 2; Q_P = Q2_H2O / divisor; return; }
    if (lettura > pos1) { QMP = 1; cr3 = 1; Q_P = Q1_H2O / divisor; return; }
}
