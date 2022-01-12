#include <Arduino.h>
#include <Adafruit_VL53L0X.h>

//right motor
#define PWM_PIN_FORWARD_RIGHT 5
#define PWM_PIN_BACKWARD_RIGHT 4
#define ENABLE_PIN_RIGHT_1 7
#define ENABLE_PIN_RIGHT_2 6

//left motor
#define PWM_PIN_FORWARD_LEFT 9
#define PWM_PIN_BACKWARD_LEFT 8
#define ENABLE_PIN_LEFT_1 11
#define ENABLE_PIN_LEFT_2 10

//ir sensors 
#define IR_SENSOR_LEFT A0
#define IR_SENSOR_RIGHT A1

//variables for IR-Sensors
bool leftIRSensor;
bool rightIRSensor;

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
#define LOX_ADDRESS_LEFT 0x30
#define LOX_ADDRESS_RIGHT 0x31
#define LOX_ADDRESS_MIDDLE 0x32

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
    Serial.print(measureLeft.RangeMilliMeter);
    distanceLeft = measureLeft.RangeMilliMeter;
  } else {
    Serial.print(F("Out of range"));
  }
  
  Serial.print(F(" "));

  // print sensor two reading
  Serial.print(F("Right: "));
  if(measureRight.RangeStatus != 4) {
    Serial.print(measureRight.RangeMilliMeter);
    distanceRight = measureRight.RangeMilliMeter;
  } else {
    Serial.print(F("Out of range"));
  }

  Serial.print(F(" "));

  // print sensor two reading
  Serial.print(F("Middle: "));
  if(measureMiddle.RangeStatus != 4) {
    Serial.print(measureMiddle.RangeMilliMeter);
    distanceMiddle = measureMiddle.RangeMilliMeter;
  } else {
    Serial.print(F("Out of range"));
  }
  
  Serial.println();
}

void readIRSensors(){
  if(digitalRead(IR_SENSOR_LEFT) == LOW){
    
    leftIRSensor = true;

  } else{

    leftIRSensor = false;

  }

  if(digitalRead(IR_SENSOR_RIGHT) == LOW){
    
    rightIRSensor = true;

  } else{

    rightIRSensor = false;

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
  digitalWrite(PWM_PIN_FORWARD_RIGHT, speed);
  digitalWrite(PWM_PIN_BACKWARD_RIGHT, 0);
  digitalWrite(PWM_PIN_FORWARD_LEFT, speed);
  digitalWrite(PWM_PIN_BACKWARD_LEFT, 0);

}

void turn(char direction){
  if(direction == 'r'){
    digitalWrite(PWM_PIN_FORWARD_RIGHT, 0);
    digitalWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    digitalWrite(PWM_PIN_FORWARD_LEFT, 50);
    digitalWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }else if(direction == 'l'){
    digitalWrite(PWM_PIN_FORWARD_RIGHT, 50);
    digitalWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    digitalWrite(PWM_PIN_FORWARD_LEFT, 0);
    digitalWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
  

}


void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) { delay(1); }

  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(IR_SENSOR_RIGHT, INPUT);

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
  digitalWrite(ENABLE_PIN_LEFT_2, HIGH);
  digitalWrite(ENABLE_PIN_RIGHT_1, HIGH);
  digitalWrite(ENABLE_PIN_RIGHT_2, HIGH);

}

void loop() {
  read_tof_sensors();
  delay(50);
}