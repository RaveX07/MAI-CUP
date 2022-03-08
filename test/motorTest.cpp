#include <Arduino.h>

//right motor
#define PWM_PIN_FORWARD_RIGHT 5
#define PWM_PIN_BACKWARD_RIGHT 4
#define ENABLE_PIN_RIGHT_1 6

//left motor
#define PWM_PIN_FORWARD_LEFT 9
#define PWM_PIN_BACKWARD_LEFT 8
#define ENABLE_PIN_LEFT_1 10

void drive(int speed){
  digitalWrite(ENABLE_PIN_LEFT_1, HIGH);
  digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);
  analogWrite(PWM_PIN_FORWARD_RIGHT, speed);
  analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
  analogWrite(PWM_PIN_FORWARD_LEFT, speed);
  analogWrite(PWM_PIN_BACKWARD_LEFT, 0);

}

void setup(){
    Serial.begin(115200);

    pinMode(PWM_PIN_FORWARD_LEFT, OUTPUT);
    pinMode(PWM_PIN_BACKWARD_LEFT, OUTPUT);
    pinMode(ENABLE_PIN_LEFT_1, OUTPUT);

    pinMode(PWM_PIN_FORWARD_RIGHT, OUTPUT);
    pinMode(PWM_PIN_BACKWARD_RIGHT, OUTPUT);
    pinMode(ENABLE_PIN_RIGHT_1, OUTPUT);

    digitalWrite(ENABLE_PIN_LEFT_1, HIGH);
    digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);

}

void loop(){
    drive(125);
}
