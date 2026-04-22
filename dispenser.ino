#include <Arduino.h>
#include "dispenser.h"
#include "wifi.h"  // necessario per sendSSEUpdate()

/* --- VARIABILI DELLA LOGICA DEL DISPENSER --- */
const int    motor   = 3,   // Pin del motore
            valore   = 255, // Velocita' del motore
              led1   = 11,  // Pin LED debug
           trigPin   = 10,  // Pin TRIG del sensore a ultrasuoni
           echoPin   = 9;   // Pin ECHO del sensore a ultrasuoni

int         // Valori dell'acqua da erogare (attualmente gli stessi per le crocchette)
            Q1_H2O   = 30000,   // 350ml
            Q2_H2O   = 60000,   // 700ml
            Q3_H2O   = 103000,  // 1200ml
            Q4_H2O   = 172000,  // 2000ml
            Q5_H2O   = 257000,  // 3000ml

             // Valori delle crocchette dall'erogare (attualmente non in uso)
             Q1_cr   = 21000,   // 100g attenzione...da verificare con prove sperimentali
             Q2_cr   = 42000,   // 180g
             Q3_cr   = 72000,   // 250g
             Q4_cr   = 120000,  // 400g
             Q5_cr   = 180000,  // 800g

             // Valori impostati dai commutatori
             tempo   = 0, // Intervallo di Erogazione
            tempoR   = 0, // Tempo di ritardo della prima erogazione 

              // Valori delle posizioni dei commutatori
              pos5   = 750, 
              pos4   = 600, 
              pos3   = 450, 
              pos2   = 300, 
              pos1   = 150,

               // Variabili CR utilizzati per memorizzare le posizioni fisiche dei commutatori interpretate
               cr1   = 0, 
           cr1_old   = 0,
               cr2   = 0, 
           cr2_old   = 0,
               cr3   = 0, 
           cr3_old   = 0,

             
             cr1_r   = 1,  
             cr2_r   = 1, 
             cr3_r   = 1,

                 h   = 0, 
                 m   = 0, 
                 s   = 0,
               Q_P   = 0,
              giro   = 100,
             H2O_P   = 1000,
           divisor   = 0,

          duration   = 0,
          distance   = 0,
           livello   = 50,
   sensore_livello   = 0,

  tensioneBatterie   = 0,

     effective_cr1, 
     effective_cr2, 
     effective_cr3;

bool test             = true;
bool remoto           = false;
bool reset_tempi;
bool prima_erogazione;

/* --- FUNZIONE SETUP DELLA LOGICA DEL DISPENSER --- */
void dispenserSetup() {
  pinMode(motor, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  analogReference(AR_EXTERNAL);
  analogReadResolution(10);
}

/* --- FUNZIONI DI LETTURA COMMUTATORI --- */
void confronti_sw1(int lettura) {
  if (lettura > pos5) {
    tempoR = 4;
    cr1 = 5;
    return;
  }
  if (lettura > pos4) {
    tempoR = 3;
    cr1 = 4;
    return;
  }
  if (lettura > pos3) {
    tempoR = 2;
    cr1 = 3;
    return;
  }
  if (lettura > pos2) {
    tempoR = 1;
    cr1 = 2;
    return;
  }
  if (lettura > pos1) {
    tempoR = 0;
    if (cr1 != 1) { h = m = s = 0; }
    cr1 = 1;
    return;
  }
}

void confronti_sw2(int lettura) {
  if (lettura > pos5) {
    tempo = 12;
    divisor = 2;
    cr2 = 5;
    return;
  }
  if (lettura > pos4) {
    tempo = 8;
    divisor = 3;
    cr2 = 4;
    return;
  }
  if (lettura > pos3) {
    tempo = 6;
    divisor = 4;
    cr2 = 3;
    return;
  }
  if (lettura > pos2) {
    tempo = 4;
    divisor = 6;
    cr2 = 2;
    return;
  }
  if (lettura > pos1) {
    tempo = 3;
    divisor = 8;
    cr2 = 1;
    return;
  }
}

void confronti_sw3(int lettura) {
  if (lettura > pos5) {
    cr3 = 5;
    Q_P = Q5_H2O / divisor;
    return;
  }
  if (lettura > pos4) {
    cr3 = 4;
    Q_P = Q4_H2O / divisor;
    return;
  }
  if (lettura > pos3) {
    cr3 = 3;
    Q_P = Q3_H2O / divisor;
    return;
  }
  if (lettura > pos2) {
    cr3 = 2;
    Q_P = Q2_H2O / divisor;
    return;
  }
  if (lettura > pos1) {
    cr3 = 1;
    Q_P = Q1_H2O / divisor;
    return;
  }
}

/* --- LOOP DEL DISPENSER: logica principale di funzionamento --- */
void dispenserLoop() {
  /* --- ASSEGNAZIONE VARIABILI DA USARE PER LA LOGICA --- */
  effective_cr1 = remoto ? cr1_r : cr1;
  effective_cr2 = remoto ? cr2_r : cr2;
  effective_cr3 = remoto ? cr3_r : cr3;
 
  /* --- MODALITA' TEST: riduciamo i tempi di funzionamento, per verificarlo --- */
  if (test == true) {
    delay(16);
  } 
  else {
    delay(1000);
  }

  // Incrementiamo il timer
  s++;

  /* --- EROGAZIONE: in funzione o per il tempo o se si tratta della prima erogazione senza ritardo --- */
  if ((h >= tempo) || (prima_erogazione)) {
    prima_erogazione = false;

    // Eroghiamo
    digitalWrite(led1, HIGH);
    analogWrite(motor, valore);

    delay(Q_P);

    // Fermiamo l'erogazione
    analogWrite(motor, 0);
    digitalWrite(led1, LOW);

    // Controlliamo il livello del cibo/acqua rimasto tramite il sensore a ultrasuoni
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = (duration * 0.0343) / 2;

    // Debug per conoscere il valore distanza
    Serial.print("Distance: ");
    Serial.println(distance);

    // Controllo cibo/acqua finito?
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

    // Dopo l'erogazione annuliamo tutto
    h = 0;
    m = 0;
    s = 0;
  }
  
  /* --- TIMER SOFTWARE INTEGRATO --- */
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

  /* --- SALVATAGGIO VALORI VECCHI --- */
  if (remoto == false) {
    cr1_old = cr1;
    cr2_old = cr2;
    cr3_old = cr3;
  } 
  else {
    cr1_old = effective_cr1;
    cr2_old = effective_cr2;
    cr3_old = effective_cr3;
  }

  /* --- TODO: Da rivedere --- */
  if (!remoto) {
    // Leggi commutatori
    confronti_sw1(analogRead(A0));
    confronti_sw2(analogRead(A1));
    confronti_sw3(analogRead(A2));
  } 
  else {
    // aggiornamento variabili di erogazione da remoto
    switch (effective_cr1) {
      case 1:
        tempoR = 0;
        if (!reset_tempi) {
          h = m = s = 0;
          reset_tempi = true;
        }
        prima_erogazione = true;
        break;
      case 2:
        tempoR = 1;
        reset_tempi = false;
        break;
      case 3:
        tempoR = 2;
        reset_tempi = false;
        break;
      case 4:
        tempoR = 3;
        reset_tempi = false;
        break;
      case 5:
        tempoR = 4;
        reset_tempi = false;
        break;
      default:
        tempoR = 0;
        reset_tempi = false;
        break;
    }
    switch (effective_cr2) {
      case 1:
        tempo = 3;
        divisor = 8;
        break;
      case 2:
        tempo = 4;
        divisor = 6;
        break;
      case 3:
        tempo = 6;
        divisor = 4;
        break;
      case 4:
        tempo = 8;
        divisor = 3;
        break;
      case 5:
        tempo = 12;
        divisor = 2;
        break;
      default:
        tempo = 4;
        divisor = 6;
        break;
    }
    switch (effective_cr3) {
      case 1:
        Q_P = Q1_H2O / divisor;
        break;
      case 2:
        Q_P = Q2_H2O / divisor;
        break;
      case 3:
        Q_P = Q3_H2O / divisor;
        break;
      case 4:
        Q_P = Q4_H2O / divisor;
        break;
      case 5:
        Q_P = Q5_H2O / divisor;
        break;
      default:
        Q_P = Q3_H2O / divisor;
        break;
    }
  }

  /* --- HOOK SSE: invia aggiornamento se cambia qualcosa --- */
  if ((cr1 != cr1_old || cr2 != cr2_old || cr3 != cr3_old) && !remoto) {
    sendSSEUpdate();
  }

  /* --- LAMPEGGI DI FEEDBACK --- */
  if (effective_cr1 != cr1_old) {
    for (int i = 0; i < effective_cr1; i++) {
      digitalWrite(led1, HIGH);
      delay(100);
      digitalWrite(led1, LOW);
      delay(100);
    }
  }
  if (effective_cr2 != cr2_old) {
    for (int i = 0; i < effective_cr2; i++) {
      digitalWrite(led1, HIGH);
      delay(100);
      digitalWrite(led1, LOW);
      delay(100);
    }
  }
  if (effective_cr3 != cr3_old) {
    for (int i = 0; i < effective_cr3; i++) {
      digitalWrite(led1, HIGH);
      delay(100);
      digitalWrite(led1, LOW);
      delay(100);
    }
  }

}

