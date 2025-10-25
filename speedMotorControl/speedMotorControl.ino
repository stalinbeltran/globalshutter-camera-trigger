#include <EEPROM.h>

int PWM_PIN = 3;

//statuses:
const float VOLTAGE_FACTOR = 1;
const int DELAY_UPPER_LIMIT = 1600;  //MILLISECONDS
const int VOLTAGE_UPPER_LIMIT = 254;    //4.96 V aprox. 254
const int VOLTAGE_STEPS = 84;
const int LOWER_VOLTAGE_LIMIT = VOLTAGE_UPPER_LIMIT - VOLTAGE_STEPS;    //2.65 V aprox. 170
const float DELAY_FACTOR = (0.5*DELAY_UPPER_LIMIT)/VOLTAGE_STEPS;  // 9 MILLISECONDS

unsigned int outputVoltage = 0;
int stepDelay = 0;
int delayIncrement = 0;
int voltageIncrement = 0;
int step = 0;
int stepIncrement = 0;

unsigned long beginningPulse0 = micros();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, 0);
  startMotor();
}

void startMotor(){    //fast start to avoid resonance in the motor
  while(outputVoltage <= VOLTAGE_UPPER_LIMIT){
    outputVoltage = outputVoltage + 1;
    analogWrite(PWM_PIN, outputVoltage);
    delay(10);
  }
}

void loop() {
	step = step + stepIncrement;
    if (outputVoltage >=VOLTAGE_UPPER_LIMIT){
	  step = 0;									//reset on upper limit
	  stepIncrement = 1;
    }
    else if (outputVoltage <= LOWER_VOLTAGE_LIMIT){
	  stepIncrement = -1;
    }
	voltageIncrement = step*VOLTAGE_FACTOR;
	delayIncrement = step*DELAY_FACTOR;
    outputVoltage = VOLTAGE_UPPER_LIMIT - voltageIncrement;
    stepDelay = DELAY_UPPER_LIMIT + delayIncrement;
	
    analogWrite(PWM_PIN, outputVoltage);
    delay(stepDelay);
}


void printValue(String name, unsigned long value){
    Serial.print(" " + name + ":");
    Serial.print(value);
    Serial.println(" ");
}
