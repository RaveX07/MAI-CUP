#include <Arduino.h>
#include <Adafruit_VL53L0X.h>

#define LOX_ADDRESS_LEFT 0x52
#define LOX_ADDRESS_RIGHT 0x53
#define LOX_ADDRESS_MIDDLE 0x54

// set the pins to shutdown
#define SHT_LOX_LEFT 22
#define SHT_LOX_RIGHT 23
#define SHT_LOX_MIDDLE 24

//variables for the TOF-Sensors
int distanceLeft = 0;
int distanceRight = 0;
int distanceMiddle = 0;


// objects for the vl53l0x
Adafruit_VL53L0X loxLeft = Adafruit_VL53L0X();
Adafruit_VL53L0X loxMiddle = Adafruit_VL53L0X();
Adafruit_VL53L0X loxRight = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measureLeft;
VL53L0X_RangingMeasurementData_t measureRight;
VL53L0X_RangingMeasurementData_t measureMiddle;

void initTOFs() {
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
  if(!loxLeft.begin()) {
    Serial.println(F("Failed to boot left VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2 and resetting lOX 3
  digitalWrite(SHT_LOX_RIGHT, HIGH);
  delay(10);

  //initing LOX2
  if(!loxRight.begin()) {
    Serial.println(F("Failed to boot right VL53L0X"));
    while(1);
  }

  //activating LOX 3
  digitalWrite(SHT_LOX_MIDDLE, HIGH);
  delay(10);

  //initinh LOX 3
  if(!loxMiddle.begin()) {
    Serial.println(F("Failed to boot middle VL53L0X"));
    while(1);
  }

}

void read_tof_sensors() {
  
  loxLeft.rangingTest(&measureLeft, false); // pass in 'true' to get debug data printout!
  loxRight.rangingTest(&measureRight, false); // pass in 'true' to get debug data printout!
  loxMiddle.rangingTest(&measureMiddle, false); // pass in 'true' to get debug data printout!

  // print sensor one reading
  Serial.print("Left: ");
  if(measureLeft.RangeStatus != 4) {     // if not out of range
    Serial.print(measureLeft.RangeMilliMeter * 10);
    distanceLeft = measureLeft.RangeMilliMeter * 10;
  } else {
    Serial.print(F("Out of range"));
  }
  
  Serial.print(F(" "));

  // print sensor two reading
  Serial.print("Right: ");
  if(measureRight.RangeStatus != 4) {
    Serial.print(measureRight.RangeMilliMeter * 10);
    distanceRight = measureRight.RangeMilliMeter * 10;
  } else {
    Serial.print(F("Out of range"));
  }

  Serial.print(F(" "));

  // print sensor three reading
  Serial.print("Middle: ");
  if(measureMiddle.RangeStatus != 4) {
    Serial.print(measureMiddle.RangeMilliMeter * 10);
    distanceMiddle = measureMiddle.RangeMilliMeter * 10;
  } else {
    Serial.print(F("Out of range"));
  }
  
  Serial.println();
}

void setup(){
    Serial.begin(115200);

    pinMode(SHT_LOX_LEFT, OUTPUT);
    pinMode(SHT_LOX_RIGHT, OUTPUT);
    pinMode(SHT_LOX_MIDDLE, OUTPUT);

    Serial.println(F("TOF-Sensors Shutdown pins inited..."));

    digitalWrite(SHT_LOX_LEFT, LOW);
    digitalWrite(SHT_LOX_RIGHT, LOW);
    digitalWrite(SHT_LOX_MIDDLE, LOW);

    Serial.println(F("All TOF-Sensors in reset mode...(pins are low)"));
    
    
    Serial.println(F("Starting TOF-Sensors..."));
    initTOFs(); 
}

void loop(){
    read_tof_sensors();
    delay(100);
}