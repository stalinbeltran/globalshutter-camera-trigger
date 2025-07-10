#include <EEPROM.h>
int SIGNAL_PIN = A0;
int TRIGGER_PIN = 2;
//saved values dirs:
int SIGNAL_LOW_DIR = 0;
int SIGNAL_HIGH_DIR = 1;
int PULSE_WIDTH_DIR = 2;

int signalLow = 48;
int signalHigh = 55;
int pulseWidth = 16000;

bool signalIsHigh = true;
int signal = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("starting");
  pinMode(SIGNAL_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  //read signal "borders"
  signalLow = EEPROM.read(SIGNAL_LOW_DIR);
  signalHigh = EEPROM.read(SIGNAL_HIGH_DIR);
  pulseWidth = EEPROM.read(PULSE_WIDTH_DIR);
  
  digitalWrite(TRIGGER_PIN, 1);   //set to high, ready to trigger
}

void loop() {
  // put your main code here, to run repeatedly:
  signal = analogRead(SIGNAL_PIN);
  if (signalIsHigh){
    if (signal < signalLow){
      cameraTrigger(pulseWidth);
      Serial.println("trigger");
      Serial.println(signal);
      signalIsHigh = false;
    }
  } else{
    if (signal > signalHigh)
      signalIsHigh = true;    //time to reset, ready for a new trigger
  }

  //Serial.println(signal);
}

void cameraTrigger(int pulseWidth){
  digitalWrite(TRIGGER_PIN, 0);
  delayMicroseconds(pulseWidth);
  digitalWrite(TRIGGER_PIN, 1);
}