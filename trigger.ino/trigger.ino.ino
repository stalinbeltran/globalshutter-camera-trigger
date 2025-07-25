#include <EEPROM.h>
int SIGNAL_PIN = A0;
int TRIGGER_PIN = 2;
//saved values dirs:
int SIGNAL_LOW_DIR = 0;
int SIGNAL_HIGH_DIR = 2;
int PULSE_WIDTH_DIR = 4;

int signalLow = 0;
int signalHigh = 0;
int pulseWidth = 0;

bool signalIsHigh = true;
int signal = 0;
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
  pinMode(SIGNAL_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  //read signal "borders"
  EEPROM.get(SIGNAL_LOW_DIR, signalLow);
  EEPROM.get(SIGNAL_HIGH_DIR, signalHigh);
  EEPROM.get(PULSE_WIDTH_DIR, pulseWidth);
  
  digitalWrite(TRIGGER_PIN, 1);   //set to high, ready to trigger
//  Serial.print("pulseWidth: ");
//  Serial.println(pulseWidth);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available()){
    Serial.println("command");
    dir = Serial.readString().toInt();
    Serial.println("address: " + String(dir) + " \nPlease type value:");
    delay(4000);                //you have time to write the value
    dirValue = Serial.readString().toInt();
    EEPROM.put(dir, dirValue);
    Serial.println(dirValue);
  }

  signal = analogRead(SIGNAL_PIN);
  if (signalIsHigh){
    if (signal < signalLow){
      //cameraTrigger(pulseWidth);
      //Serial.println("trigger pulseWidth: " + String(pulseWidth) + " signal: " + String(signal));
      signalIsHigh = false;
    }
  } else{
    if (signal > signalHigh){
      signalIsHigh = true;    //time to reset, ready for a new trigger
      //Serial.println("reset signal: " + String(signal));
    }
  }

  if (signal > 46){
    //Serial.println(3);
    if (cont == 0){
      final = millis();
      elapsed = final - inicio;
      if (elapsed > 50){
        cameraTrigger(pulseWidth);
        //Serial.println(elapsed);
        inicio = final;
      }
    }
    cont++;
   }
  if (signal < 40){
    //Serial.println(-3);
    if (cont > 0){
      //Serial.println(cont);
      cont = 0;
    }
  }

  Serial.println(signal);
  //delay(10);
  referencia++;
  if (referencia > 300){
    Serial.println(0);
    referencia = 0;
  }
}

void cameraTrigger(int pulseWidth){
  //Serial.println("trigger");
  digitalWrite(TRIGGER_PIN, 0);
  delayMicroseconds(pulseWidth);
  digitalWrite(TRIGGER_PIN, 1);
}
