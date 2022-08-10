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

const int tolerance = 1.07;

int x = 0;

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
  analogWrite(PWM_PIN_FORWARD_RIGHT, speed * tolerance);
  analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
  analogWrite(PWM_PIN_FORWARD_LEFT, speed);
  analogWrite(PWM_PIN_BACKWARD_LEFT, 0);

}

void rotate(char direction, int speed)
{
    if (direction == 'l') {
        analogWrite(PWM_PIN_FORWARD_RIGHT, 0);
        analogWrite(PWM_PIN_BACKWARD_RIGHT, speed);
        analogWrite(PWM_PIN_FORWARD_LEFT, speed);
        analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
    }
    else if (direction == 'r') {
        analogWrite(PWM_PIN_FORWARD_RIGHT, speed);
        analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
        analogWrite(PWM_PIN_FORWARD_LEFT, 0);
        analogWrite(PWM_PIN_BACKWARD_LEFT, speed);
    }
}

void turnCustom(char direction, int speedMax, int speedMin)
{
    if (direction == 'l') {
        analogWrite(PWM_PIN_FORWARD_RIGHT, speedMin);
        analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
        analogWrite(PWM_PIN_FORWARD_LEFT, speedMax);
        analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
    }
    else if (direction == 'r') {
        analogWrite(PWM_PIN_FORWARD_RIGHT, speedMax);
        analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
        analogWrite(PWM_PIN_FORWARD_LEFT, speedMin);
        analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
    }
}

void readHallSensors(){
    magnetLeft = digitalRead(HALL_SENSORS_LEFT);
    magnetRight = digitalRead(HALL_SENSORS_RIGHT); 

    if(magnetLeft == 0 || magnetRight == 0){
        digitalWrite(15 , HIGH);
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
//    if (valueLeft >= 1023 && valueRight >= 1023){
//        if(distanceFront < 35){
//            delay(1000);
//            while(true){
//                drive(0);
//                delay(1000);
//            }
//        }
//    }
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

        turnCustom('l', 50, 46); // if distance to wall is higher on the right, turn a bit to the left

    } else if (distanceRight > distanceLeft) {

        Serial.println("Distance left < distance right");

        turnCustom('r', 50, 41); // if distance to wall is higher on the right, turn a bit to the right
    }else {
      drive(50);
    }
}

void turn(char direction, bool instant){
    if (x < 3){
  
        if(direction == 'r'){
        
        if(instant == false){
            while(distanceFront > 15){
            drive(50);
            readAllSensors();
            }
        }
        
        while(distanceFront < 50){
            rotate('r', 50);
            readAllSensors();
        }
    
        delay(200); 
    
        drive(50);
    
        while(distanceRight > 35){
            readAllSensors();
        }
    
        readAllSensors();
        
        } else if(direction == 'l'){
    
    
        if(instant == false){
            while(distanceFront > 15){
            drive(50);
            readAllSensors();
            }       
        }   
        
        while(distanceFront < 50){
            rotate('l', 50);
            readAllSensors();
        }
    
        delay(200); 
    
        drive(50);
    
        while(distanceLeft > 35){
            readAllSensors();
        }
    
        readAllSensors();
        
        }

        x ++;
    }else {
        while(true){
            drive(0);
            delay(100000);
        }
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
    if(distanceLeft > 40 && distanceRight < 40 && distanceFront < 35){    // if in left corner turn left 
        turn('l', false);
    } else if(distanceRight > 40 && distanceLeft < 40 && distanceFront < 35){   //if in right corner turn right
        turn('r', false);
    } else if(distanceRight > 40 && distanceLeft < 40 && distanceFront > 35){     // if no corner drive forward with a slight turn left
        while(distanceRight > 40){

            readAllSensors();
            
            drive(50);

            if(lineRight == true){

                drive(50);

                delay(100);
              
                rotate('r', 50);

                delay(500);
        
                turn('r', true);
            }
        }
        // these functions are for the case that there's a straight path to take but the bot can't balance out if there's no wall 
    } else if(distanceLeft > 40 && distanceRight < 40 && distanceFront > 35){     // if no corner drive forward with a slight turn right
        while(distanceLeft > 40){

            readAllSensors();
            
            drive(50);
            
            if(lineLeft == true){

                drive(50);

                delay(100);

                rotate('l', 50);

                delay(500);
                
                turn('l', true);

            }
        }
    } else if(distanceLeft > 40 && distanceRight > 40 && distanceFront < 40){
        while(!lineLeft && !lineRight){
            drive(50);
            readAllSensors();
        }

        if(lineLeft == true){
            turn('l', false);     
        } else if(lineRight == true) {
            turn('r', false);
        } 
    } 
}

void setup(){
    Serial.begin(9500);
    pinMode(PWM_PIN_FORWARD_LEFT, OUTPUT);
    pinMode(PWM_PIN_BACKWARD_LEFT, OUTPUT);
    pinMode(ENABLE_PIN_LEFT_1, OUTPUT);

    pinMode(PWM_PIN_FORWARD_RIGHT, OUTPUT);
    pinMode(PWM_PIN_BACKWARD_RIGHT, OUTPUT);
    pinMode(ENABLE_PIN_RIGHT_1, OUTPUT);

    digitalWrite(ENABLE_PIN_LEFT_1, HIGH);
    digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);

    pinMode(LED, OUTPUT);

    digitalWrite(15, LOW);

    Serial.println("Setup complete ");

    drive(40);

    delay(1500);
}

void loop(){

    mainCode();     
}