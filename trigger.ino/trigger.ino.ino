#include <EEPROM.h>
int SIGNAL_PIN0 = A0;
int TRIGGER_PIN = 2;

//statuses:
const int RESET = 0;
const int MOBILE_COMING = 1;
const int FIXED_COMING = 2;
const int FIXED_WAITING = 3;
int status = RESET;

//signal "borders"
int signalLow = 46;
int signalHigh = 52;
int pulseWidth = 31;    //31 is the minimum pulse width that works

int signal0 = 0;    //sensor for moving mirror
int counter0 = 0;   //status triggered in the begining
unsigned long beginningPulse0 = micros();
unsigned long periodBeginning = micros();
unsigned long beginningFixedWaiting = micros();
unsigned long halfPeriod = 0;
unsigned long quarterPeriod = 0;
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
  periodMeasurement(periodBeginning, beginningPulse0, halfPeriod, quarterPeriod);
  fixedMirrorPulse(beginningFixedWaiting, quarterPeriod);
}

void periodMeasurement(unsigned long &periodBeginning, unsigned long &actualPulseBeginning, 
  unsigned long &halfPeriod, unsigned long &quarterPeriod){
  if(periodBeginning != actualPulseBeginning){
    halfPeriod = (unsigned long)actualPulseBeginning - (unsigned long)periodBeginning;
    halfPeriod = halfPeriod/1000;     //microseconds to milliseconds
    periodBeginning = actualPulseBeginning;
    quarterPeriod = halfPeriod/2;
  }
}

void fixedMirrorPulse(unsigned long &beginningFixedWaiting, unsigned long &quarterPeriod){
  unsigned long final, dif;
  if (status == FIXED_COMING){
    
    beginningFixedWaiting = millis();
    status = FIXED_WAITING;
    return;
  }
  if (status == FIXED_WAITING){
    final = millis();
    dif = (unsigned long)final - (unsigned long)beginningFixedWaiting;
    if(dif >= quarterPeriod){         //arm is out of the way
      cameraTrigger(pulseWidth);      //camera capture fixed mirror
      status = RESET;
    }
    return;
  }
}


void cameraTrigger(int pulseWidth){
  digitalWrite(TRIGGER_PIN, 0);
  delayMicroseconds(pulseWidth);
  digitalWrite(TRIGGER_PIN, 1);
}

void processSignal(int signal, int &counter, unsigned long &beginningPulse, 
  unsigned long &widthPulse, int &status){
  unsigned long widthPulseActual = 0, microsActual = 0;
  if (signal > signalHigh){
    counter++;               //sensed pulse is now high
    if (counter == 1){
      microsActual = (unsigned long)micros();
      widthPulseActual = (unsigned long)microsActual - (unsigned long)beginningPulse;
      if (widthPulseActual > widthPulse*2){     //widthPulseActual should be longer if it has no mirror
        status = MOBILE_COMING;
      }
      
      widthPulse = widthPulseActual;    //save sensed pulse width
    }
  }
  if (signal < signalLow){		//sensed pulse is now low
    if (counter > 0){        	//previously was high, so mobile arm is now detected
      beginningPulse = (unsigned long)micros();    //start measuring the arm "width" when crossing the sensor
      if (status == MOBILE_COMING){		//detected mobile mirror
        cameraTrigger(pulseWidth);    //pulse to camera capture mobile mirror
        status = FIXED_COMING;          //ready to read fixed mirror
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
