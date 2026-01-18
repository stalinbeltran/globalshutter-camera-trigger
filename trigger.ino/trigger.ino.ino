#include <EEPROM.h>
int SIGNAL_MOBILE_MIRROR = A0;
int TRIGGER_PIN = 2;

//statuses:
const int RESET = 0;
const int MOBILE_COMING = 1;
int status = RESET;

//signalMobileMirror "borders"
int signalLow = 46;
int signalHigh = 52;
const int PULSE_WIDTH = 50;    //31 is the minimum pulse width that works

int signalMobileMirror = 0;    //sensor for moving mirror
int counter0 = 0;   //status triggered in the begining
unsigned long beginningMobileMirrorPulse = micros();
unsigned long periodBeginning = beginningMobileMirrorPulse;
unsigned long halfPeriod = 0;
unsigned long quarterPeriod = 0;
unsigned long widthMobileMirrorPulse = 9999;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SIGNAL_MOBILE_MIRROR, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  digitalWrite(TRIGGER_PIN, 1);   //set to high, ready to trigger the optoisolator
}

void loop() {
  signalMobileMirror = analogRead(SIGNAL_MOBILE_MIRROR);
  processMobileSignal();
  periodMeasurement();
}

void periodMeasurement(){
  if(periodBeginning != beginningMobileMirrorPulse){
    halfPeriod = (unsigned long)beginningMobileMirrorPulse - (unsigned long)periodBeginning;
    halfPeriod = halfPeriod/1000;     //microseconds to milliseconds
    periodBeginning = beginningMobileMirrorPulse;
    quarterPeriod = halfPeriod/2;
  }
}

void cameraTrigger(){
  digitalWrite(TRIGGER_PIN, 0);
  delayMicroseconds(PULSE_WIDTH);
  digitalWrite(TRIGGER_PIN, 1);
}

void processMobileSignal(){
  unsigned long widthPulseActual = 0, microsActual = 0;
  if (signalMobileMirror > signalHigh){
    counter0++;               //sensed pulse is now high
    if (counter0 == 1){
      microsActual = (unsigned long)micros();
      widthPulseActual = (unsigned long)microsActual - (unsigned long)beginningMobileMirrorPulse;
      if (widthPulseActual > widthMobileMirrorPulse*2){     //widthPulseActual should be longer if it has no mirror (instead has a long tail)
        if (status == RESET) status = MOBILE_COMING;		//mandatory RESET before MOBILE_COMING, to stop triggering if not fixed pulse
      }
      
      widthMobileMirrorPulse = widthPulseActual;    //save sensed pulse width
    }
	return;
  }
  if (signalMobileMirror < signalLow){		//sensed pulse is now low
    if (counter0 > 0){        	//previously was high, so mobile arm is now detected
      beginningMobileMirrorPulse = (unsigned long)micros();    //start measuring the arm "width" when crossing the sensor
      if (status == MOBILE_COMING){		//detected mobile mirror
        cameraTrigger();    	//pulse to camera capture mobile mirror
        status = RESET;
      }
      counter0 = 0;           	//negative signalMobileMirror detected
    }
	return;
  }
}

void printValue(String name, unsigned long value){
    Serial.print(" " + name + ":");
    Serial.print(value);
    Serial.println(" ");
}
