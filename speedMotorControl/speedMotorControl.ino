#include <EEPROM.h>

int PWM_PIN = 3;

//statuses:
const int VOLTAGE_INCREMENT = 1;
const int DELAY_BASE_INCREMENT = -2;  //MILLISECONDS
const int DELAY_BASE = 60;  //MILLISECONDS
const int UPPER_VOLTAGE_LIMIT = 254;    //4.96 V aprox.
const int LOWER_VOLTAGE_LIMIT = 170;    //2.65 V aprox.

unsigned int output = 0;
int stepDelay = 0;
int delayIncrement = 0;
int voltageIncrement = 0;

unsigned long beginningPulse0 = micros();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, 0);
  startMotor();
}

void startMotor(){    //fast start to avoid resonance in the motor
  while(output <= UPPER_VOLTAGE_LIMIT){
    output = output + 1;
    analogWrite(PWM_PIN, output);
    delay(10);
  }
}

void loop() {
    if (output >=UPPER_VOLTAGE_LIMIT){
      delayIncrement = -1*DELAY_BASE_INCREMENT;
      voltageIncrement = -1*VOLTAGE_INCREMENT;
      stepDelay = DELAY_BASE;
    }
    else if (output <= LOWER_VOLTAGE_LIMIT){
      delayIncrement = DELAY_BASE_INCREMENT;
      voltageIncrement = VOLTAGE_INCREMENT;
    } 
    output = output + voltageIncrement;
    stepDelay = stepDelay + delayIncrement;
    analogWrite(PWM_PIN, output);
    delay(stepDelay);

  
}


void printValue(String name, unsigned long value){
    Serial.print(" " + name + ":");
    Serial.print(value);
    Serial.println(" ");
}
