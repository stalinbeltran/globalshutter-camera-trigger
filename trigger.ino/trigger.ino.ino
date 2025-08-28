#include <EEPROM.h>
int SIGNAL_PIN0 = A0;
int SIGNAL_PIN1 = A1;
int TRIGGER_PIN = 2;


//signal "borders"
int signalLow = 40;
int signalHigh = 50;
int pulseWidth = 80;

bool signalIsHigh = true;
int signal0 = 0;
int signal1 = 0;
int dir = 0;
int dirValue = 0;
int cont = 0;
int inicio = millis();
int final = 0;
int elapsed = 0;
int referencia = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("starting");
  pinMode(SIGNAL_PIN0, INPUT);
  pinMode(SIGNAL_PIN1, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  
  digitalWrite(TRIGGER_PIN, 0);   //set to low, ready to trigger by optoisolator
//  Serial.print("pulseWidth: ");
//  Serial.println(pulseWidth);
}

void loop() {

  signal0 = analogRead(SIGNAL_PIN0);
  signal1 = analogRead(SIGNAL_PIN1);

  if (signal0 > signalHigh || signal1 > signalHigh){
    cont++;
   }
  if (signal0 < signalLow || signal1 < signalLow){
    if (cont > 0){
      cameraTrigger(pulseWidth);
      final = millis();
      elapsed = final - inicio;
      //Serial.println(elapsed);
      inicio = final;
      cont = 0;
    }
  }

  //Serial.println(signal);
  //delay(10);
  referencia++;
  if (referencia > 300){
    //Serial.println(0);
    referencia = 0;
  }
}

void cameraTrigger(int pulseWidth){
  digitalWrite(TRIGGER_PIN, 1);
  delayMicroseconds(pulseWidth);
  digitalWrite(TRIGGER_PIN, 0);
}

void processSignal(int signal, int &counter){
  
}
