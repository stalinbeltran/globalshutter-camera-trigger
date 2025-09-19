#include <EEPROM.h>
int SIGNAL_PIN0 = A0;
//int SIGNAL_PIN1 = A1;
int TRIGGER_PIN = 2;

//statuses:
const int RESET = 0;
const int MOBILE_COMING = 1;
int status = RESET;

//signal "borders"
int signalLow = 46;
int signalHigh = 50;
int pulseWidth = 31;    //31 is the minimum pulse width that works

int signal0 = 0;    //sensor for moving mirror
int signal1 = 0;    //sensor for static mirror
int counter0 = 0;   //status triggered in the begining
int counter1 = 0;   //status triggered in the begining
int beginningPulse0 = 0;
int widthPulse0 = 0;

int referencia = 0;
int factor = 15;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SIGNAL_PIN0, INPUT);
  //pinMode(SIGNAL_PIN1, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  digitalWrite(TRIGGER_PIN, 1);   //set to low, ready to trigger the optoisolator
}

void loop() {
  int previousElapsed = 0;
  signal0 = analogRead(SIGNAL_PIN0);
  processSignal(signal0, counter0, beginningPulse0, widthPulse0, status);


}

void cameraTrigger(int pulseWidth){
  digitalWrite(TRIGGER_PIN, 0);
  delayMicroseconds(pulseWidth);
  digitalWrite(TRIGGER_PIN, 1);
}

void processSignal(int signal, int &counter, int &beginningPulse, int &widthPulse, int &status){
  int final, widthPulseActual = 0;
  if (signal > signalHigh){
    counter++;               //sensed pulse is now high
    if (counter == 1){
      widthPulseActual = micros() - beginningPulse;
      if (widthPulseActual > widthPulse*2){     //widthPulseActual should be longer
        status = MOBILE_COMING;
//        printValue("widthPulseActual", widthPulseActual);
//        printValue("widthPulse", widthPulse);
      } else{
        status = RESET;
      }
      
      widthPulse = widthPulseActual;    //save sensed pulse width
    }
  }
  if (signal < signalLow){		//sensed pulse is now low
    if (counter > 0){        	//detected mobile arm
      if (status == MOBILE_COMING){		//detected mobile mirror
        cameraTrigger(pulseWidth);    //pulse mobile mirror
        delay(35);                    //replace 35 by 1/4 T
        cameraTrigger(pulseWidth);    //pulse fixed mirror (triggered entirely by time)
        status = RESET;
      }
      counter = 0;           //trigger once only
      beginningPulse = micros();    //start measuring the arm "width" when crossing the sensor
    }
  }
}

void printValue(String name, int value){
    Serial.print(" " + name + ":");
    Serial.print(value);
    Serial.println(" ");
}
