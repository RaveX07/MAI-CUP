#include <Wire.h>
#include <VL53L0X.h>

#define LOX_ADDRESS_LEFT 0x52
#define LOX_ADDRESS_RIGHT 0x53
#define LOX_ADDRESS_MIDDLE 0x54

// set the pins to shutdown
#define SHT_LOX_LEFT 22
#define SHT_LOX_RIGHT 23
#define SHT_LOX_MIDDLE 24

//variables for the TOF-sensorLefs
int distanceLeft = 0;
int distanceRight = 0;
int distanceMiddle = 0;

VL53L0X sensorLeft;
VL53L0X sensorRight;
VL53L0X sensorMiddle;


// Uncomment this line to use long range mode. This
// increases the sensitivity of the sensorLef and extends its
// potential range, but increases the likelihood of getting
// an inaccurate reading because of reflections from objects
// other than the intended target. It works best in dark
// conditions.

//#define LONG_RANGE


// Uncomment ONE of these two lines to get
// - higher speed at the cost of lower accuracy OR
// - higher accuracy at the cost of lower speed

//#define HIGH_SPEED
#define HIGH_ACCURACY


void setup()
{
    Serial.begin(115200);
    Wire.begin();

    digitalWrite(SHT_LOX_LEFT, LOW);
    digitalWrite(SHT_LOX_MIDDLE, LOW);
    digitalWrite(SHT_LOX_RIGHT, LOW);
    
    digitalWrite(SHT_LOX_LEFT, HIGH);
    sensorLeft.setTimeout(500);
    if (!sensorLeft.init())
    {
        Serial.println("Failed to detect and initialize sensorLeft!");
        while (1) {}
    }
    sensorLeft.setAddress(LOX_ADDRESS_LEFT);
    Serial.println("Initialized left Sensor");

    digitalWrite(SHT_LOX_MIDDLE, HIGH);
    sensorMiddle.setTimeout(500);
    if (!sensorMiddle.init())
    {
        Serial.println("Failed to detect and initialize sensorMiddle!");
        while (1) {}
    }
    sensorMiddle.setAddress(LOX_ADDRESS_MIDDLE);
    Serial.println("Initialized middle Sensor");

    digitalWrite(SHT_LOX_RIGHT, HIGH);
    sensorRight.setTimeout(500);
    if (!sensorRight.init())
    {
        Serial.println("Failed to detect and initialize sensorRight!");
        while (1) {}
    }
    sensorRight.setAddress(LOX_ADDRESS_RIGHT);
    Serial.println("Initialized right Sensor");

#if defined LONG_RANGE
    // lower the return signal rate limit (default is 0.25 MCPS)
    sensorLeft.setSignalRateLimit(0.1);
    // increase laser pulse periods (defaults are 14 and 10 PCLKs)
    sensorLeft.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    sensorLeft.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

    // lower the return signal rate limit (default is 0.25 MCPS)
    sensorMiddle.setSignalRateLimit(0.1);
    // increase laser pulse periods (defaults are 14 and 10 PCLKs)
    sensorMiddle.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    sensorMiddle.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

    // lower the return signal rate limit (default is 0.25 MCPS)
    sensorMiddle.setSignalRateLimit(0.1);
    // increase laser pulse periods (defaults are 14 and 10 PCLKs)
    sensorMiddle.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    sensorMiddle.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
    // reduce timing budget to 20 ms (default is about 33 ms)
    sensorLeft.setMeasurementTimingBudget(20000);
    sensorMiddle.setMeasurementTimingBudget(20000);
    sensorRight.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
    // increase timing budget to 200 ms
    sensorLeft.setMeasurementTimingBudget(200000);
    sensorMiddle.setMeasurementTimingBudget(200000);
    sensorRight.setMeasurementTimingBudget(200000);
#endif
}

void loop()
{
    Serial.print("Left:  ");
    Serial.print(sensorLeft.readRangeSingleMillimeters());
    if (sensorLeft.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

    Serial.println();

    Serial.print("Middle:  ");
    Serial.print(sensorMiddle.readRangeSingleMillimeters());
    if (sensorMiddle.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

    Serial.println();

    Serial.print("Right:  ");
    Serial.print(sensorRight.readRangeSingleMillimeters());
    if (sensorRight.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

    Serial.println();
}