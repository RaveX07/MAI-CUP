#include <SR04.h>
#include <Arduino.h>
int LED = 15;

//right motor
#define PWM_PIN_FORWARD_RIGHT 5
#define PWM_PIN_BACKWARD_RIGHT 4
#define ENABLE_PIN_RIGHT_1 6

//left motor
#define PWM_PIN_FORWARD_LEFT 8
#define PWM_PIN_BACKWARD_LEFT 9
#define ENABLE_PIN_LEFT_1 10

//left US sensor
const int triggerLeft = 40;
const int echoLeft = 38;
long distanceLeft;
SR04 sr04 = SR04(echoLeft, triggerLeft);

//front US sensor works
const int triggerFront = 50;
const int echoFront = 48;
long distanceFront;
SR04 sr05 = SR04(echoFront, triggerFront);

//right US sensor works
const int triggerRight = 51;
const int echoRight = 49;
long distanceRight;
SR04 sr06 = SR04(echoRight, triggerRight);

// right IR sensor
const int IRSensorRight = A1;
int valueRight = 0;

//left IR sensor
const int IRSensorLeft = A0;
int valueLeft = 0;

//variables for IR-Sensors
bool lineLeft;
bool lineRight;

void drive(int speed){
  digitalWrite(ENABLE_PIN_LEFT_1, HIGH);
  digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);
  analogWrite(PWM_PIN_FORWARD_RIGHT, speed*1.1);
  analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
  analogWrite(PWM_PIN_FORWARD_LEFT, speed);
  analogWrite(PWM_PIN_BACKWARD_LEFT, 0);

}

void rotate(char direction, int speed)
{
  if (direction == 'r')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, 0);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, speed);
    analogWrite(PWM_PIN_FORWARD_LEFT, speed);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
  else if (direction == 'l')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, speed);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, 0);
    analogWrite(PWM_PIN_BACKWARD_LEFT, speed);
  }
}

void turnCustom(char direction, int speedMax, int speedMin)
{
  if (direction == 'l')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, speedMin);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, speedMax);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
  else if (direction == 'r')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, speedMax);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, speedMin);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
}


void readIRSensors(){
  valueLeft = analogRead(IRSensorLeft);
  valueRight = analogRead(IRSensorRight);
  Serial.print(valueLeft);
  if (valueLeft >= 700){
    lineLeft = true;
    Serial.println("; line detected left");
  } else {
    lineLeft = false;
    Serial.println("; no line left");
  }
  Serial.print(valueRight);
  if (valueRight >= 700){
    lineRight = true;
    Serial.println("; line detected right");
  } else {
    lineRight = false;
    Serial.println("; no line right");
  }

}




void turn(){

    char direction;
    
    while (distanceRight < 40 && distanceLeft < 40)
    { // while no curve is detected: drive forward
        drive(100);
        readUSSensors();
        readIRSensors();
    }

    if(distanceLeft > 40){
        direction = 'l';
    } else {
        direction = 'r';
    }

    switch (direction)
    {
    case 'r':

        rotate('r', 100);

        delay(500); // rotate for 0.5 seconds

        drive(100); // drive forward for 0.5 seconds

        delay(500);
        break;
    
    case 'l':

        rotate('l', 100);

        delay(500); // rotate for 0.5 seconds

        drive(100); // drive forward for 0.5 seconds

        delay(500);

    default:
        break;
    }

}


void readUSSensors(){
    distanceLeft = sr04.Distance();
    distanceFront = sr05.Distance();
    distanceRight = sr06.Distance();
    Serial.println("left:" + String(distanceLeft) + "cm; front:" + String(distanceFront) + "cm; right:" + String(distanceRight) +  "cm");  
}

void setup(){
    Serial.begin(9600);
    pinMode(PWM_PIN_FORWARD_LEFT, OUTPUT);
    pinMode(PWM_PIN_BACKWARD_LEFT, OUTPUT);
    pinMode(ENABLE_PIN_LEFT_1, OUTPUT);

    pinMode(PWM_PIN_FORWARD_RIGHT, OUTPUT);
    pinMode(PWM_PIN_BACKWARD_RIGHT, OUTPUT);
    pinMode(ENABLE_PIN_RIGHT_1, OUTPUT);

    digitalWrite(ENABLE_PIN_LEFT_1, HIGH);
    digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);

    pinMode(LED, OUTPUT);

    Serial.println("Setup complete ");
}

void loop(){
    
    

}
