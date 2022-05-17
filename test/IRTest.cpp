#include <Arduino.h>

//ir sensors 
#define IR_SENSOR_LEFT A0
#define IR_SENSOR_RIGHT A1

//variables for IR-Sensors
int IRSensorLeft;
int IRSensorRight;

bool lineLeft;
bool lineRight;

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

void setup(){

    Serial.begin(115200);

    pinMode(IR_SENSOR_LEFT, INPUT);
    pinMode(IR_SENSOR_RIGHT, INPUT);
}

void loop() {
    readIRSensors();
    Serial.print("Rechts:  ");
    Serial.println(IR_SENSOR_LEFT);
    Serial.print("Links:  ");
    Serial.println(IR_SENSOR_RIGHT);
    
}