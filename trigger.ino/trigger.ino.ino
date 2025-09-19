#include <EEPROM.h>
int SIGNAL_PIN0 = A0;
int TRIGGER_PIN = 2;

//statuses:
const int RESET = 0;
const int MOBILE_COMING = 1;
int status = RESET;

//signal "borders"
int signalLow = 46;
int signalHigh = 52;
int pulseWidth = 31;    //31 is the minimum pulse width that works

int signal0 = 0;    //sensor for moving mirror
int counter0 = 0;   //status triggered in the begining
unsigned long beginningPulse0 = micros();
unsigned long widthPulse0 = 9999;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SIGNAL_PIN0, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  digitalWrite(TRIGGER_PIN, 1);   //set to high, ready to trigger the optoisolator
}

void loop() {
  signal0 = analogRead(SIGNAL_PIN0);
  processSignal(signal0, counter0, beginningPulse0, widthPulse0, status);
}

void cameraTrigger(int pulseWidth){
  digitalWrite(TRIGGER_PIN, 0);
  delayMicroseconds(pulseWidth);
  digitalWrite(TRIGGER_PIN, 1);
}

void processSignal(int signal, int &counter, unsigned long &beginningPulse, unsigned long &widthPulse, int &status){
  unsigned long widthPulseActual = 0, microsActual = 0;
  if (signal > signalHigh){
    counter++;               //sensed pulse is now high
    if (counter == 1){
      microsActual = (unsigned long)micros();
      widthPulseActual = (unsigned long)microsActual - (unsigned long)beginningPulse;
//        printValue("widthPulse", widthPulse);
//        printValue("microsActual", (unsigned long)microsActual);
//        printValue("beginningPulse", (unsigned long)beginningPulse);
        printValue("widthPulseActual", (unsigned long)widthPulseActual);
      if (widthPulseActual > widthPulse*2){     //widthPulseActual should be longer
        status = MOBILE_COMING;
      } else{
        status = RESET;
      }
      
      widthPulse = widthPulseActual;    //save sensed pulse width
    }
    return;
  }
  if (signal < signalLow){		//sensed pulse is now low
    if (counter > 0){        	//previously was high, so mobile arm is now detected
      beginningPulse = (unsigned long)micros();    //start measuring the arm "width" when crossing the sensor
      
//        printValue("---beginningPulse", beginningPulse);
      if (status == MOBILE_COMING){		//detected mobile mirror
        cameraTrigger(pulseWidth);    //pulse mobile mirror
        //delay(35);                    //replace 35 by 1/4 T
        //cameraTrigger(pulseWidth);    //pulse fixed mirror (triggered entirely by time)
        status = RESET;
      }
      counter = 0;           //trigger once only
    }
  }
}

void printValue(String name, unsigned long value){
    Serial.print(" " + name + ":");
    Serial.print(value);
    Serial.println(" ");
}
