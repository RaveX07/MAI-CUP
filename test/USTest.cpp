#include <Arduino.h>
#include <HCSR04.h>

//US-Sensors
#define ECHO_MIDDLE 48
#define TRIGGER_MIDDLE 50

#define ECHO_RIGHT 49
#define TRIGGER_RIGHT 51

#define ECHO_LEFT 38
#define TRIGGER_LEFT 40

UltraSonicDistanceSensor distanceSensorMiddle(ECHO_MIDDLE, TRIGGER_MIDDLE);
UltraSonicDistanceSensor distanceSensorRight(ECHO_RIGHT, TRIGGER_RIGHT);
UltraSonicDistanceSensor distanceSensorLeft(ECHO_LEFT, TRIGGER_LEFT);


void setup () {
    Serial.begin(115200);  // We initialize serial connection so that we could print values from sensor.
}

void loop () {
    // Every 500 miliseconds, do a measurement using the sensor and print the distance in centimeters.
    Serial.println(distanceSensorMiddle.measureDistanceCm());
    Serial.println(distanceSensorRight.measureDistanceCm());
    Serial.println(distanceSensorLeft.measureDistanceCm());

    delay(500);
}