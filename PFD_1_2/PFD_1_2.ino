/*TODO: passare posizioni virtuali agli cr.*/
int motor=3;      // Motore Cibo
int valore=255;   // max=255 60 per crocchette 255 per pompa
int led1 = 11;
int lettura = 0;
int Q1_H2O=21000; //delay pos1 sw3 per 350ml ==> 60s=1000ml
int Q2_H2O=42000; //700ml
int Q3_H2O=72000; //1200ml
int Q4_H2O=120000; //2000ml
int Q5_H2O=180000; //3000ml
int QMP = 0;
int QP = 0;
int QM              = 0;
int QMA             = 0;
int QA              = 0;
int ledbuzzer       = 11;
int tempo           = 0;
int tempoR          = 0;
int pos5            = 750;  //lettura della 5 posizione
int pos4            = 600;  //lettura della 4 posizione
int pos3            = 450;  //lettura della 3 posizione
int pos2            = 300;  //lettura della 2 posizione
int pos1            = 150;  //lettura della 1 posizione
int cr1             = 0;    // Posizione Commutatore Rotativo 1 Software (1...5)
int cr1_old         = 0;
int cr2             = 0;    // Posizione Commutatore Rotativo 2 Software (1...5)
int cr2_old         = 0;
int cr3             = 0;    // Posizione Commutatore Rotativo 3 Software (1...5)
int cr3_old         = 0;

int cr1_r           = 0;    // Posizione Commutatore Rotativo 1 Remoto (1...5)
int cr2_r           = 0;    // Posizione Commutatore Rotativo 2 Remoto (1...5)
int cr3_r           = 0;    // Posizione Commutatore Rotativo 3 Remoto (1...5)

int h               = 0;
int m               = 0;
int s               = 0;

int Q_P = 0;                // tempo di erogazione pompa
int Q_mot;                  // tempo erogazione motore
int giro            = 100;  // grammi per giro
int H2O_P           = 1000; // 1000ml/60s
int divisor         = 0;

const int trigPin   = 10;
const int echoPin   = 9;

int duration        = 0;
int distance        = 0;
int livello         = 50;
int sensore_livello = 0;

bool remoto = false;


void setup()
{
  pinMode(motor, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(ledbuzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  analogReference(AR_EXTERNAL);
  analogReadResolution(10);
  Serial.begin(9600);
}


//void loop()
//{
 
//analogWrite(motor, valore);
 //valore=analogRead(A0);
 //valore=valore/4;
  //digitalWrite(LED_BUILTIN, HIGH);
  //delay(1000); // Wait for 1000 millisecond(s)
  //digitalWrite(LED_BUILTIN, LOW);
  //delay(1000); // Wait for 1000 millisecond(s)
//}






void loop()
 {
 
  delay(16); //deve essere 1000

  s=s+1;

  if (h>=tempo){
    digitalWrite(led1, HIGH);
    analogWrite(motor, valore);
    delay(Q_P); // quantità indicata da cr3
    analogWrite(motor, 0);
    digitalWrite(led1, LOW);
    digitalWrite(trigPin, LOW);  
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);  
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);


    duration = pulseIn(echoPin, HIGH);
    distance = (duration*.0343)/2;


    Serial.print(" Distance: ");
    Serial.println(distance);
    if (distance>livello) {
      digitalWrite(led1, HIGH);
      delay(100);
      digitalWrite(led1, LOW);
      delay(100);
      digitalWrite(led1, HIGH);
      delay(100);
      digitalWrite(led1, LOW);
      delay(500);
      digitalWrite(led1, HIGH);
      delay(500);
      digitalWrite(led1, LOW);
    }
    delay(1000);
    h=0;
    m=0;
    s=0;
  }

  if (s==60)  {
    m=m+1;
    s=0;
 
   // Serial.print(" m:");
   // Serial.print (m);
   // analogWrite(motor, valore);
   // delay(2680);
   // analogWrite(motor, 0);
    if (m==60) {
      h=h+1;
      m=0;
      s=0;
      //Serial.print(" h:");
      //Serial.print (h);
      if (h==24) { //superfluo
        h=0;
        m=0;
        s=0;
      }
    }
  }


cr1_old=cr1;
cr2_old=cr2;
cr3_old=cr3;  

confronti_sw1(analogRead(A0));
confronti_sw2(analogRead(A1));
confronti_sw3(analogRead(A2));
lettura=analogRead(A0);
//Serial.println(lettura);


if (cr1!=cr1_old)
{   //Serial.println (lettura);
    switch(cr1){
     case 1:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     


     break;


     case 2:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 3:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 4:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
      delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 5:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
      delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;
    }
 }


if (cr2!=cr2_old)
{
    switch(cr2){
     case 1:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 2:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 3:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 4:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
      delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 5:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
      delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;
    }
 }
if (cr3!=cr3_old)
{
    switch(cr3){
     case 1:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 2:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 3:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 4:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
      delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;


     case 5:
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
      delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     delay(100);
     digitalWrite(led1, HIGH);
     delay(100);
     digitalWrite(led1, LOW);
     break;
    }
 }
 }

// Definizione delle funzioni confronti per commutatori rotativi singoli
void confronti_sw1(int lettura){
  if (lettura>pos5){


    tempoR=4;
    cr1=5;
    return;//esce dalla funzione confronti
  }


  if (lettura>pos4){


    tempoR=3;
    cr1=4;


    return;//esce dalla funzione confronti
  }


  if (lettura>pos3){


    tempoR=2;
    cr1=3;
    return;//esce dalla funzione confronti
  }


  if (lettura>pos2){


    tempoR=1;
    cr1=2;
    return;//esce dalla funzione confronti
  }


  if (lettura>pos1){
  
    tempoR=0; //senza ritardo
    if (cr1!=1){
    h=0;
    m=0;
    s=0;
    }
    cr1=1;
      //reset tempi
    return;//esce dalla funzione confronti
  }
}

void confronti_sw2(int lettura){
  if (lettura>pos5){
    tempo=5; //12;
    divisor=2; //divisorisore 2 volte al giorno
    cr2=5;
    return;//esce dalla funzione confronti
  }


  if (lettura>pos4){
    tempo=5; //8;
    divisor=3; //divisorisore 3 volte al giorno
    cr2=4;


    return;//esce dalla funzione confronti
  }


  if (lettura>pos3){
    tempo=3; //6;
    divisor=4; //divisorisore 4 volte al giorno
    cr2=3;
    return;//esce dalla funzione confronti
  }


  if (lettura>pos2){
    tempo=2; //4;
    divisor=6; //divisorisore 6 volte al giorno
    cr2=2;
    return;//esce dalla funzione confronti
  }


  if (lettura>pos1){
    tempo=1; //3; //ogni 3 ore, 8 volte al giorno
    divisor=8; //divisorisore 8 volte al giorno
    cr2=1;
    return;//esce dalla funzione confronti
  }
}

void confronti_sw3(int lettura){
  if (lettura>pos5){
    QMA=1;
    cr3=5;
    Q_P=int(Q5_H2O/divisor);
    return;//esce dalla funzione confronti
  }


  if (lettura>pos4){
    QA=1;
    cr3=4;
    Q_P=int(Q4_H2O/divisor);
    return;//esce dalla funzione confronti
  }


  if (lettura>pos3){
    QM=1;
    cr3=3;
    Q_P=int(Q3_H2O/divisor);
    return;//esce dalla funzione confronti
  }


  if (lettura>pos2){
    QP=1;
    cr3=2;
    Q_P=int(Q2_H2O/divisor);
    return;//esce dalla funzione confronti
  }


  if (lettura>pos1){
    QMP=1;
    cr3=1;
    Q_P=int(Q1_H2O/divisor);
    return;//esce dalla funzione confronti
  }
}
