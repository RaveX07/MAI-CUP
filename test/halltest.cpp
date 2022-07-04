#include <Arduino.h>

#define LEFT_HALL_SENSOR 12
#define RIGHT_HALL_SENSOR 13

void setup(){
    Serial.begin(115200);

    pinMode(LEFT_HALL_SENSOR, INPUT);
    pinMode(RIGHT_HALL_SENSOR, INPUT);

}

void loop(){
    Serial.println("#################################");
    Serial.println("LEFT HALL SENSOR:  " + String(digitalRead(LEFT_HALL_SENSOR)));
    Serial.println("RIGHT HALL SENSOR:  " + String(digitalRead(RIGHT_HALL_SENSOR)));
}