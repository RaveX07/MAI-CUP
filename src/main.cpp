#include <Arduino.h>
#include <Adafruit_VL53L0X.h>

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

//variables for the TOF-Sensors
int distanceLeft = 0;
int distanceRight = 0;
int distanceMiddle = 0;

// address we will assign if dual sensor is present
#define LOX_ADDRESS_LEFT 0x52
#define LOX_ADDRESS_RIGHT 0x53
#define LOX_ADDRESS_MIDDLE 0x54

// set the pins to shutdown
#define SHT_LOX_LEFT 22
#define SHT_LOX_RIGHT 23
#define SHT_LOX_MIDDLE 24

// objects for the vl53l0x
Adafruit_VL53L0X loxLeft = Adafruit_VL53L0X();
Adafruit_VL53L0X loxMiddle = Adafruit_VL53L0X();
Adafruit_VL53L0X loxRight = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measureLeft;
VL53L0X_RangingMeasurementData_t measureRight;
VL53L0X_RangingMeasurementData_t measureMiddle;



/*
    Reset all sensors by setting all of their XSHUT pins low for delay(10), then set all XSHUT high to bring out of reset
    Keep sensor #1 awake by keeping XSHUT pin high
    Put all other sensors into shutdown by pulling XSHUT pins low
    Initialize sensor #1 with lox.begin(new_i2c_address) Pick any number but 0x29 and it must be under 0x7F. Going with 0x30 to 0x3F is probably OK.
    Keep sensor #1 awake, and now bring sensor #2 out of reset by setting its XSHUT pin high.
    Initialize sensor #2 with lox.begin(new_i2c_address) Pick any number but 0x29 and whatever you set the first sensor to
 */
void setID() {
  // all reset
  digitalWrite(SHT_LOX_LEFT, LOW);    
  digitalWrite(SHT_LOX_RIGHT, LOW);
  digitalWrite(SHT_LOX_MIDDLE, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX_LEFT, HIGH);
  digitalWrite(SHT_LOX_RIGHT, HIGH);
  digitalWrite(SHT_LOX_MIDDLE, HIGH);
  delay(10);

  // activating LOX1 and resetting LOX2 and LOX3
  digitalWrite(SHT_LOX_LEFT, HIGH);
  digitalWrite(SHT_LOX_RIGHT, LOW);
  digitalWrite(SHT_LOX_MIDDLE, LOW);

  // initing LOX1
  if(!loxLeft.begin(LOX_ADDRESS_LEFT)) {
    Serial.println(F("Failed to boot left VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2 and resetting lOX 3
  digitalWrite(SHT_LOX_RIGHT, HIGH);
  digitalWrite(SHT_LOX_MIDDLE, LOW);
  delay(10);

  //initing LOX2
  if(!loxRight.begin(LOX_ADDRESS_RIGHT)) {
    Serial.println(F("Failed to boot right VL53L0X"));
    while(1);
  }

  //activating LOX 3
  digitalWrite(SHT_LOX_MIDDLE, HIGH);
  delay(10);

  //initinh LOX 3
  if(!loxMiddle.begin(LOX_ADDRESS_MIDDLE)) {
    Serial.println(F("Failed to boot middle VL53L0X"));
    while(1);
  }

}

void read_tof_sensors() {
  
  loxLeft.rangingTest(&measureLeft, false); // pass in 'true' to get debug data printout!
  loxRight.rangingTest(&measureRight, false); // pass in 'true' to get debug data printout!
  loxMiddle.rangingTest(&measureMiddle, false); // pass in 'true' to get debug data printout!

  // print sensor one reading
  Serial.print(F("Left: "));
  if(measureLeft.RangeStatus != 4) {     // if not out of range
    Serial.print(measureLeft.RangeMilliMeter * 10);
    distanceLeft = measureLeft.RangeMilliMeter * 10;
  } else {
    Serial.print(F("Out of range"));
  }
  
  Serial.print(F(" "));

  // print sensor two reading
  Serial.print(F("Right: "));
  if(measureRight.RangeStatus != 4) {
    Serial.print(measureRight.RangeMilliMeter * 10);
    distanceRight = measureRight.RangeMilliMeter * 10;
  } else {
    Serial.print(F("Out of range"));
  }

  Serial.print(F(" "));

  // print sensor two reading
  Serial.print(F("Middle: "));
  if(measureMiddle.RangeStatus != 4) {
    Serial.print(measureMiddle.RangeMilliMeter * 10);
    distanceMiddle = measureMiddle.RangeMilliMeter * 10;
  } else {
    Serial.print(F("Out of range"));
  }
  
  Serial.println();
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

  pinMode(SHT_LOX_LEFT, OUTPUT);
  pinMode(SHT_LOX_RIGHT, OUTPUT);
  pinMode(SHT_LOX_MIDDLE, OUTPUT);

  Serial.println(F("TOF-Sensors Shutdown pins inited..."));

  digitalWrite(SHT_LOX_LEFT, LOW);
  digitalWrite(SHT_LOX_RIGHT, LOW);
  digitalWrite(SHT_LOX_MIDDLE, LOW);

  Serial.println(F("All TOF-Sensors in reset mode...(pins are low)"));
  
  
  Serial.println(F("Starting TOF-Sensors..."));
  setID();  

  digitalWrite(ENABLE_PIN_LEFT_1, HIGH);
  digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);

  Serial.println("Setup complete ");

}

void loop() {
  read_tof_sensors();                 //read all sensors
  readHallSensors();
  readIRSensors();

  Serial.println("Reading all Sensors");

  
  if(magnetDetectedLeft){             //if magnet is detected on the right

    Serial.println("Magnet detected on the left");

    while(distanceRight < 40){        //while no curve is detected: drive forward
      drive(75); 
      
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    rotate('r', 70);

    delay(500);                       //rotate for 0.5 seconds

    drive(75);                        //drive forward for 0.5 seconds

    delay(500);
    
  } else if(magnetDetectedRight){     //if magnet is detected on the right   

    Serial.println("Magnet detected on the right");

    while(distanceLeft < 40){         //while no curve is detected: drive forward
      drive(75);
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    rotate('l', 70);                  

    delay(500);                       //rotate for 0.5 seconds

    drive(75);                        //drive forward for 0.5 seconds

    delay(500);
  }


  if(lineRight == false && lineLeft == false){  //if there's no black line detected

    Serial.println("No line detected");

    if(distanceRight < distanceLeft || distanceLeft < 40 || distanceRight < 40){

      Serial.println("Distance left > distance right"); 

      turnCustom('l', 80, 65);        //if distance to wall is higher on the right, turn a bit to the left

    } else if(distanceRight > distanceLeft || distanceLeft < 40 || distanceRight < 40){

      Serial.println("Distance left > distance right"); 

      turnCustom('r', 80, 65);        //if distance to wall is higher on the right, turn a bit to the right

    } else {

      Serial.println("No wall on one side"); 

      drive(100);

    }
  } else if(lineRight == true && lineLeft == false){  //if there's a black line on the right

    Serial.println("Line detected one the right"); 

    while(distanceRight < 40){         //while no curve is detected: drive forward
      drive(75);
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    rotate('r', 70);                  

    delay(500);                       //rotate for 0.5 seconds

    drive(75);                        //drive forward for 0.5 seconds

    delay(500);


  } else if(lineRight == false && lineLeft == true){

    Serial.println("Line detected one the left"); 

    while(distanceLeft < 40){         //while no curve is detected: drive forward
      drive(75);
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    rotate('l', 70);                  

    delay(500);                       //rotate for 0.5 seconds

    drive(75);                        //drive forward for 0.5 seconds

    delay(500);
    
  }


  if(distanceMiddle < 30){

    Serial.println("Wall in front"); 

    while(distanceLeft < 40 && distanceRight < 40){
      drive(75);
    }

    delay(200);                       //keep driving forward for 0.2 seconds

    if(distanceLeft >= 40){
      rotate('l', 70);                  

      delay(500);                       //rotate for 0.5 seconds

      drive(75);                        //drive forward for 0.5 seconds

      delay(500);

    } else if(distanceRight >= 40){
      rotate('r', 70);                  

      delay(500);                       //rotate for 0.5 seconds

      drive(75);                        //drive forward for 0.5 seconds

      delay(500);

    }

  }
  delay(50);

}