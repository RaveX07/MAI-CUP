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
const int IRSensorRight = A0;
int valueRight = 0;

//left IR sensor
const int IRSensorLeft = A1;
int valueLeft = 0;

//variables for IR-Sensors
bool lineLeft;
bool lineRight;

//Hall Sensors
#define HALL_SENSORS_LEFT 12
#define HALL_SENSORS_RIGHT 13

int magnetRight = 0;
int magnetLeft = 0;


void drive(int speed){
  digitalWrite(ENABLE_PIN_LEFT_1, HIGH);
  digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);
  analogWrite(PWM_PIN_FORWARD_RIGHT, speed);
  analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
  analogWrite(PWM_PIN_FORWARD_LEFT, speed);
  analogWrite(PWM_PIN_BACKWARD_LEFT, 0);

}

void rotate(char direction, int speed)
{
  if (direction == 'l')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, 0);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, speed);
    analogWrite(PWM_PIN_FORWARD_LEFT, speed);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
  else if (direction == 'r')
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

void readHallSensors(){
    magnetLeft = digitalRead(HALL_SENSORS_LEFT);
    magnetRight = digitalRead(HALL_SENSORS_RIGHT); 

    if(magnetLeft == 1 || magnetRight == 1){
        digitalWrite(15 , HIGH);
    } else {
        digitalWrite(15, LOW);
    }
}

void readUSSensors(){
    distanceLeft = sr04.Distance();
    distanceFront = sr05.Distance();
    distanceRight = sr06.Distance();
    Serial.println("left:" + String(distanceLeft) + "cm; front:" + String(distanceFront) + "cm; right:" + String(distanceRight) +  "cm");  
}



void readIRSensors(){
  valueLeft = analogRead(IRSensorLeft);
  valueRight = analogRead(IRSensorRight);
  Serial.print("LEFT:  " + String(valueLeft));
  if (valueLeft >= 700){
    lineLeft = true;
    Serial.println("; line detected left");
  } else {
    lineLeft = false;
    Serial.println("; no line left");
  }
  Serial.print("RIGHT:  " + String(valueRight));
  if (valueRight >= 700){
    lineRight = true;
    Serial.println("; line detected right");
  } else {
    lineRight = false;
    Serial.println("; no line right");
  }

}

void readAllSensors(){
    readIRSensors();
    readUSSensors();
    readHallSensors();
}

void balance(){
    readAllSensors();
    if (distanceRight < distanceLeft)
    {

        Serial.println("Distance left > distance right");

        turnCustom('l', 60, 55); // if distance to wall is higher on the right, turn a bit to the left

    } else if (distanceRight > distanceLeft) {

        Serial.println("Distance left < distance right");

        turnCustom('r', 60, 55); // if distance to wall is higher on the right, turn a bit to the right
    }else {
      drive(50);
    }
}

void turn(char direction, int driveDelay){
  if(direction == 'r'){

    drive(60);
          
    delay(driveDelay);//keep driving to avoid crashing in the wall
    
    while(distanceFront < 55){
        rotate('r', 60);
        readAllSensors();
    }

    delay(70); 

    drive(60); // drive forward for 0.5 seconds

    delay(1000);

    readAllSensors();
    
  } else if(direction == 'l'){

    drive(60);
          
    delay(driveDelay);//keep driving to avoid crashing in the wall
    
    while(distanceFront < 55){
        rotate('l', 60);
        readAllSensors();
    }

    delay(70); 

    drive(60); // drive forward for 0.5 seconds

    delay(1000);

    readAllSensors();
    
  }
}

void mainCode(){  

    readAllSensors();
    while (distanceRight < 40 && distanceLeft < 40 && distanceFront > 25)
    { // while no curve is detected: drive forward
        balance();
        readAllSensors();
    }
    readAllSensors();
    if(distanceLeft > 40 && distanceFront < 35){    // if in left corner turn left 
        turn('l', 200);
    }else if(distanceRight > 40 && distanceFront < 35){   //if in right corner turn right
        turn('r', 200);
    }
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

    mainCode();     
}
