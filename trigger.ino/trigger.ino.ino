#include <EEPROM.h>
int SIGNAL_MOBILE_MIRROR = A0;
int SIGNAL_FIXED_MIRROR = A1;
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
const int PULSE_WIDTH = 31;    //31 is the minimum pulse width that works

int signalMobileMirror = 0;    //sensor for moving mirror
int signalFixedMirror = 0;    //sensor for fixed mirror
int counter0 = 0;   //status triggered in the begining
unsigned long beginningMobileMirrorPulse = micros();
unsigned long periodBeginning = micros();
unsigned long beginningFixedWaiting = micros();
unsigned long halfPeriod = 0;
unsigned long quarterPeriod = 0;
unsigned long widthMobileMirrorPulse = 9999;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SIGNAL_MOBILE_MIRROR, INPUT);
  pinMode(SIGNAL_FIXED_MIRROR, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  digitalWrite(TRIGGER_PIN, 1);   //set to high, ready to trigger the optoisolator
}

void loop() {
  signalMobileMirror = analogRead(SIGNAL_MOBILE_MIRROR);
  signalFixedMirror = analogRead(SIGNAL_FIXED_MIRROR);
  processSignal(signalMobileMirror, counter0);
  periodMeasurement(halfPeriod, quarterPeriod);
  fixedMirrorPulse(quarterPeriod, signalFixedMirror);
}

void periodMeasurement(unsigned long &halfPeriod, unsigned long &quarterPeriod){
  if(periodBeginning != beginningMobileMirrorPulse){
    halfPeriod = (unsigned long)beginningMobileMirrorPulse - (unsigned long)periodBeginning;
    halfPeriod = halfPeriod/1000;     //microseconds to milliseconds
    periodBeginning = beginningMobileMirrorPulse;
    quarterPeriod = halfPeriod/2;
  }
}

void fixedMirrorPulse(unsigned long &quarterPeriod, int signalFixedMirror){
  unsigned long final, dif;
  if (status == FIXED_WAITING){
    if (signalFixedMirror < signalLow - 6){
      final = millis();
      //dif = (unsigned long)final - (unsigned long)beginningFixedWaiting;
        cameraTrigger();      //camera capture fixed mirror
        status = RESET;
      //printValue("FixedMirror", dif);
    }
    return;
  }
}


void cameraTrigger(){
  digitalWrite(TRIGGER_PIN, 0);
  delayMicroseconds(PULSE_WIDTH);
  digitalWrite(TRIGGER_PIN, 1);
}

void processSignal(int signal, int &counter){
  unsigned long widthPulseActual = 0, microsActual = 0;
  if (signal > signalHigh){
    counter++;               //sensed pulse is now high
    if (counter == 1){
      if (status == FIXED_COMING){  //mobile pulse end, ready to read fixed mirror (here to avoid triggering while sensing mobile pulse)
        status = FIXED_WAITING;
        beginningFixedWaiting = millis();
      }          
      microsActual = (unsigned long)micros();
      widthPulseActual = (unsigned long)microsActual - (unsigned long)beginningMobileMirrorPulse;
      if (widthPulseActual > widthMobileMirrorPulse*2){     //widthPulseActual should be longer if it has no mirror
        status = MOBILE_COMING;
      }
      
      widthMobileMirrorPulse = widthPulseActual;    //save sensed pulse width
    }
	return;
  }
  if (signal < signalLow){		//sensed pulse is now low
    if (counter > 0){        	//previously was high, so mobile arm is now detected
      beginningMobileMirrorPulse = (unsigned long)micros();    //start measuring the arm "width" when crossing the sensor
      if (status == MOBILE_COMING){		//detected mobile mirror
        cameraTrigger();    	//pulse to camera capture mobile mirror
        status = FIXED_COMING;
      }
      counter = 0;           	//negative signal detected
    }
	return;
  }
}

void printValue(String name, unsigned long value){
    Serial.print(" " + name + ":");
    Serial.print(value);
    Serial.println(" ");
}
