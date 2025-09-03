#include <EEPROM.h>
int SIGNAL_PIN0 = A0;
int SIGNAL_PIN1 = A1;
int TRIGGER_PIN = 2;


//signal "borders"
int signalLow = 40;
int signalHigh = 50;
int pulseWidth = 31;    //31 is the minimum pulse width that works

int signal0 = 0;    //sensor for moving mirror
int signal1 = 0;    //sensor for static mirror
int counter0 = 0;   //status triggered in the begining
int counter1 = 0;   //status triggered in the begining

int inicio = millis();
int final = 0;
int elapsed = 0;
int referencia = 0;
int factor = 15;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SIGNAL_PIN0, INPUT);
  pinMode(SIGNAL_PIN1, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  digitalWrite(TRIGGER_PIN, 1);   //set to low, ready to trigger the optoisolator
}

void loop() {
  int previousElapsed = 0;
  signal0 = analogRead(SIGNAL_PIN0);
  signal1 = analogRead(SIGNAL_PIN1);
  processSignal(signal0, counter0);
  processSignal(signal1, counter1);


}

void cameraTrigger(int pulseWidth){
  digitalWrite(TRIGGER_PIN, 0);
  delayMicroseconds(pulseWidth);
  digitalWrite(TRIGGER_PIN, 1);
}

void processSignal(int signal, int &counter){
  if (signal > signalHigh){
    counter++;               //reset trigger
  }
  if (signal < signalLow){
    if (counter > 0){        //ready to trigger
      cameraTrigger(pulseWidth);
      counter = 0;           //triggered
      
      final = millis();
      elapsed = final - inicio;
      inicio = final;
      printValue("elapsed", elapsed);

    }
  }
}

void printValue(String name, int value){
  
    Serial.print(" " + name + ":");
    Serial.print(value);
    Serial.println(" ");
}



//  if (abs(elapsed - previousElapsed) > 950) {
//    previousElapsed = elapsed;
//    Serial.print(" signal0:");
//    Serial.print(signal0*factor);
//    Serial.print(" signal1:");
//    Serial.print(signal1*factor);
//    Serial.print(" ref:0 ");
//    
//  
//    Serial.println("");
//  }
  //Serial.println("base: 0");
  //Serial.println("base2: 80");
