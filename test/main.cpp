#include <Arduino.h>

//right motor
#define PWM_PIN_FORWARD_RIGHT 5
#define PWM_PIN_BACKWARD_RIGHT 4
#define ENABLE_PIN_RIGHT_1 6

//left motor
#define PWM_PIN_FORWARD_LEFT 9
#define PWM_PIN_BACKWARD_LEFT 8
#define ENABLE_PIN_LEFT_1 10

//ir sensors 
#define IR_SENSOR_LEFT A0
#define IR_SENSOR_RIGHT A1

//US-Sensors
#define ECHO_MIDDLE 48
#define TRIGGER_MIDDLE 50

#define ECHO_RIGHT 49
#define TRIGGER_RIGHT 51

#define ECHO_LEFT 38
#define TRIGGER_LEFT 40


//variables for the US-Sensors
const int SENSOR_MAX_RANGE = 300; // in cm
unsigned long durationMiddle;
unsigned int distanceMiddle;

unsigned long durationRight;
unsigned int distanceRight;

unsigned long durationLeft;
unsigned int distanceLeft;

//variables for IR-Sensors
int IRSensorLeft;
int IRSensorRight;

bool lineLeft;
bool lineRight;

//hall sensors 
#define HALL_SENSOR_LEFT 12
#define HALL_SENSOR_RIGHT 13

//variables for the hall-sensors
bool magnetDetectedLeft = false;
bool magnetDetectedRight = false;


void readUSSensors(){

  //read middle sensor
  digitalWrite(TRIGGER_MIDDLE, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER_MIDDLE, HIGH);
  delayMicroseconds(10);

  durationMiddle = pulseIn(ECHO_MIDDLE, HIGH);
  distanceMiddle = durationMiddle/58;

  if (distanceMiddle > SENSOR_MAX_RANGE || distanceMiddle <= 0){
    Serial.println("Out of sensor middle range!");
  } else {
    Serial.println("Distance to object middle: " + String(distanceMiddle) + " cm");
  }

  //read right sensor
  digitalWrite(TRIGGER_RIGHT, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER_RIGHT, HIGH);
  delayMicroseconds(10);

  durationRight = pulseIn(ECHO_RIGHT, HIGH);
  distanceRight = durationRight/58;

  if (distanceRight > SENSOR_MAX_RANGE || distanceRight <= 0){
    Serial.println("Out of sensor right range!");
  } else {
    Serial.println("Distance to object right: " + String(distanceRight) + " cm");
  }

  //read left sensor
  digitalWrite(TRIGGER_LEFT, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER_LEFT, HIGH);
  delayMicroseconds(10);

  durationLeft = pulseIn(ECHO_LEFT, HIGH);
  distanceLeft = durationLeft/58;

  if (distanceLeft > SENSOR_MAX_RANGE || distanceLeft <= 0){
    Serial.println("Out of sensor left range!");
  } else {
    Serial.println("Distance to object left: " + String(distanceLeft) + " cm");
  }
}

void readIRSensors(){
  IRSensorLeft = analogRead(IR_SENSOR_LEFT);
  IRSensorRight = analogRead(IR_SENSOR_RIGHT);

  if (IRSensorLeft >= 750){
    lineLeft = true;

  } else {
    lineLeft = false;

  }

  if (IRSensorRight >= 750){
    lineRight = true;

  } else {
    lineRight = false;

  }

}

void readHallSensors(){
  if(digitalRead(HALL_SENSOR_LEFT) == HIGH){
    magnetDetectedLeft = true;

  }else {
    magnetDetectedLeft = false;

  }

  if(digitalRead(HALL_SENSOR_RIGHT) == HIGH){
    magnetDetectedRight = true;

  }else {
    magnetDetectedRight = false;

  }
}



void drive(int speed){
  digitalWrite(ENABLE_PIN_LEFT_1, HIGH);
  digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);
  analogWrite(PWM_PIN_FORWARD_RIGHT, speed);
  analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
  analogWrite(PWM_PIN_FORWARD_LEFT, speed);
  analogWrite(PWM_PIN_BACKWARD_LEFT, 0);

}

void turnCustom(char direction, int speedMax, int speedMin){
  if(direction == 'r'){
    analogWrite(PWM_PIN_FORWARD_RIGHT, speedMin);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, speedMax);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }else if(direction == 'l'){
    analogWrite(PWM_PIN_FORWARD_RIGHT, speedMax);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, speedMin);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
  

}

void rotate(char direction, int speed){
  if(direction == 'r'){
    analogWrite(PWM_PIN_FORWARD_RIGHT, 0);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, speed);
    analogWrite(PWM_PIN_FORWARD_LEFT, speed);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }else if(direction == 'l'){
    analogWrite(PWM_PIN_FORWARD_RIGHT, speed);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, 0);
    analogWrite(PWM_PIN_BACKWARD_LEFT, speed);
  }
  

}


void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices

  pinMode(PWM_PIN_FORWARD_LEFT, OUTPUT);
  pinMode(PWM_PIN_BACKWARD_LEFT, OUTPUT);
  pinMode(ENABLE_PIN_LEFT_1, OUTPUT);

  pinMode(PWM_PIN_FORWARD_RIGHT, OUTPUT);
  pinMode(PWM_PIN_BACKWARD_RIGHT, OUTPUT);
  pinMode(ENABLE_PIN_RIGHT_1, OUTPUT);

  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(IR_SENSOR_RIGHT, INPUT);

  pinMode(HALL_SENSOR_LEFT, INPUT);
  pinMode(HALL_SENSOR_RIGHT, INPUT);

  pinMode(TRIGGER_MIDDLE, OUTPUT);
  pinMode(TRIGGER_RIGHT, OUTPUT);
  pinMode(ECHO_MIDDLE, INPUT);
  pinMode(ECHO_MIDDLE, INPUT);

  Serial.println("Setup complete ");

}

void loop() {
  readUSSensors();                 //read all sensors
  readHallSensors();
  readIRSensors();

  Serial.println("Reading all Sensors");

  
  if(magnetDetectedLeft){             //if magnet is detected on the right

    digitalWrite(15, HIGH);

    Serial.println("Magnet detected on the left");

    while(distanceRight < 40){        //while no curve is detected: drive forward
      drive(125); 
      
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    rotate('r', 100);

    delay(500);                       //rotate for 0.5 seconds

    drive(125);                        //drive forward for 0.5 seconds

    delay(500);
    
  } else if(magnetDetectedRight){     //if magnet is detected on the right   

    digitalWrite(15, HIGH);

    Serial.println("Magnet detected on the right");

    while(distanceLeft < 40){         //while no curve is detected: drive forward
      drive(125);
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    rotate('l', 100);                  

    delay(500);                       //rotate for 0.5 seconds

    drive(125);                        //drive forward for 0.5 seconds

    delay(500);
  }


  if(lineRight == false && lineLeft == false){  //if there's no black line detected

    Serial.println("No line detected");

    if(distanceRight < distanceLeft && distanceLeft < 40 && distanceRight < 40){

      Serial.println("Distance left > distance right"); 

      turnCustom('l', 125, 80);        //if distance to wall is higher on the right, turn a bit to the left

    } else if(distanceRight > distanceLeft && distanceLeft < 40 && distanceRight < 40){

      Serial.println("Distance left > distance right"); 

      turnCustom('r', 125, 80);        //if distance to wall is higher on the right, turn a bit to the right

    } else {

      Serial.println("No wall on one side"); 

      drive(100);

    }
  } else if(lineRight == true && lineLeft == false){  //if there's a black line on the right

    Serial.println("Line detected one the right"); 

    while(distanceRight < 40){         //while no curve is detected: drive forward
      drive(125);
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    rotate('r', 100);                  

    delay(500);                       //rotate for 0.5 seconds

    drive(125);                        //drive forward for 0.5 seconds

    delay(500);


  } else if(lineRight == false && lineLeft == true){

    Serial.println("Line detected one the left"); 

    while(distanceLeft < 40){         //while no curve is detected: drive forward
      drive(125);
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    rotate('l', 100);                  

    delay(500);                       //rotate for 0.5 seconds

    drive(125);                        //drive forward for 0.5 seconds

    delay(500);
    
  }


  if(distanceMiddle < 30){

    Serial.println("Wall in front"); 

    while(distanceLeft < 40 && distanceRight < 40 && lineLeft == false && lineRight == false){
      drive(125);
      readIRSensors();
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    if(distanceLeft >= 40){
      rotate('l', 100);                  

      delay(500);                       //rotate for 0.5 seconds

      drive(125);                        //drive forward for 0.5 seconds

      delay(500);

    } else if(distanceRight >= 40){
      rotate('r', 100);                  

      delay(500);                       //rotate for 0.5 seconds

      drive(125);                        //drive forward for 0.5 seconds

      delay(500);

    } else if(lineRight == false && lineLeft == true){

      Serial.println("Line detected one the left"); 

      while(distanceLeft < 40){         //while no curve is detected: drive forward
        drive(125);
      }

      delay(200);                       //keep driving forward for 0.2 seconds

      rotate('l', 100);                  

      delay(500);                       //rotate for 0.5 seconds

      drive(125);                        //drive forward for 0.5 seconds

      delay(500);
      
    } else if(lineRight == true && lineLeft == false){

      Serial.println("Line detected one the left"); 

      while(distanceRight < 40){         //while no curve is detected: drive forward
        drive(125);
      }

      delay(200);                       //keep driving forward for 0.2 seconds

      rotate('r', 100);                  

      delay(500);                       //rotate for 0.5 seconds

      drive(125);                        //drive forward for 0.5 seconds

      delay(500);
      
    }
    
  }

  delay(50);

}