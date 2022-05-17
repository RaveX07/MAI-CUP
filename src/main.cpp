#include <Arduino.h>
#include <HCSR04.h>

// right motor
#define PWM_PIN_FORWARD_RIGHT 5
#define PWM_PIN_BACKWARD_RIGHT 4
#define ENABLE_PIN_RIGHT 6

// left motor
#define PWM_PIN_FORWARD_LEFT 9
#define PWM_PIN_BACKWARD_LEFT 8
#define ENABLE_PIN_LEFT 10

// ir sensors
#define IR_SENSOR_LEFT A0
#define IR_SENSOR_RIGHT A1

// variables for IR-Sensors
int IRSensorLeft;
int IRSensorRight;

bool lineLeft;
bool lineRight;
bool line;

// hall sensors
#define HALL_SENSOR_LEFT 12
#define HALL_SENSOR_RIGHT 13

// variables for the hall-sensors
bool magnetDetectedLeft = false;
bool magnetDetectedRight = false;

#define LED 15

// US-Sensors
#define ECHO_MIDDLE 48
#define TRIGGER_MIDDLE 50

#define ECHO_RIGHT 49
#define TRIGGER_RIGHT 51

#define ECHO_LEFT 38
#define TRIGGER_LEFT 40

int distanceLeft;
int distanceRight;
int distanceMiddle;

UltraSonicDistanceSensor distanceSensorMiddle(ECHO_MIDDLE, TRIGGER_MIDDLE);
UltraSonicDistanceSensor distanceSensorRight(ECHO_RIGHT, TRIGGER_RIGHT);
UltraSonicDistanceSensor distanceSensorLeft(ECHO_LEFT, TRIGGER_LEFT);

void readUSSensors()
{

  Serial.println(distanceSensorMiddle.measureDistanceCm());
  Serial.println(distanceSensorRight.measureDistanceCm());
  Serial.println(distanceSensorLeft.measureDistanceCm());

  distanceMiddle = distanceSensorMiddle.measureDistanceCm();
  distanceRight = distanceSensorRight.measureDistanceCm();
  distanceLeft = distanceSensorLeft.measureDistanceCm();
  
}

void readIRSensors()
{
    IRSensorLeft = analogRead(IR_SENSOR_LEFT);
    IRSensorRight = analogRead(IR_SENSOR_RIGHT);

    if (IRSensorLeft >= 750)
    {
        lineLeft = true;
    }
    else
    {
        lineLeft = false;
    }

    if (IRSensorRight >= 750)
    {
        lineRight = true;
    }
    else
    {
        lineRight = false;
    }

    if(lineLeft || lineRight){
        line = true;
    }

}

void readHallSensors()
{
  if (digitalRead(HALL_SENSOR_LEFT) == HIGH)
  {
    magnetDetectedLeft = true;
  }
  else
  {
    magnetDetectedLeft = false;
  }

  if (digitalRead(HALL_SENSOR_RIGHT) == HIGH)
  {
    magnetDetectedRight = true;
  }
  else
  {
    magnetDetectedRight = false;
  }
}

void readAllSensors(){
    readHallSensors();
    readIRSensors();
    readUSSensors();
}
    

void drive(int speed)
{
  analogWrite(PWM_PIN_FORWARD_RIGHT, speed);
  analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
  analogWrite(PWM_PIN_FORWARD_LEFT, speed);
  analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
}

void turnCustom(char direction, int speedMax, int speedMin)
{
  if (direction == 'r')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, speedMin);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, speedMax);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
  else if (direction == 'l')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, speedMax);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, speedMin);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
}

void rotate(char direction, int speed)
{
  if (direction == 'r')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, 0);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, speed);
    analogWrite(PWM_PIN_FORWARD_LEFT, speed);
    analogWrite(PWM_PIN_BACKWARD_LEFT, 0);
  }
  else if (direction == 'l')
  {
    analogWrite(PWM_PIN_FORWARD_RIGHT, speed);
    analogWrite(PWM_PIN_BACKWARD_RIGHT, 0);
    analogWrite(PWM_PIN_FORWARD_LEFT, 0);
    analogWrite(PWM_PIN_BACKWARD_LEFT, speed);
  }
}


void turn(char direction){

    switch (direction)
    {
    case 'r':
        while (distanceRight < 40)
        { // while no curve is detected: drive forward
            drive(125);
            readAllSensors();
        }

        delay(200); // keep driving forward for 0.2 seconds

        rotate('r', 100);

        delay(500); // rotate for 0.5 seconds

        drive(125); // drive forward for 0.5 seconds

        delay(500);
        break;
    
    case 'l':
        while (distanceRight < 40)
        { // while no curve is detected: drive forward
            drive(125);
            readAllSensors();
        }

        delay(200); // keep driving forward for 0.2 seconds

        rotate('l', 100);

        delay(500); // rotate for 0.5 seconds

        drive(125); // drive forward for 0.5 seconds

        delay(500);

    default:
        break;
    }

}


void setup()
{
  Serial.begin(115200);

  // wait until serial port opens for native USB devices

  pinMode(PWM_PIN_FORWARD_LEFT, OUTPUT);
  pinMode(PWM_PIN_BACKWARD_LEFT, OUTPUT);
  pinMode(ENABLE_PIN_LEFT, OUTPUT);

  pinMode(PWM_PIN_FORWARD_RIGHT, OUTPUT);
  pinMode(PWM_PIN_BACKWARD_RIGHT, OUTPUT);
  pinMode(ENABLE_PIN_RIGHT, OUTPUT);

  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(IR_SENSOR_RIGHT, INPUT);

  pinMode(HALL_SENSOR_LEFT, INPUT);
  pinMode(HALL_SENSOR_RIGHT, INPUT);

  pinMode(TRIGGER_MIDDLE, OUTPUT);
  pinMode(TRIGGER_RIGHT, OUTPUT);
  pinMode(ECHO_MIDDLE, INPUT);
  pinMode(ECHO_MIDDLE, INPUT);

  digitalWrite(ENABLE_PIN_LEFT, HIGH);
  digitalWrite(ENABLE_PIN_RIGHT, HIGH);

  Serial.println("Setup complete ");
}

void loop()
{
    readAllSensors();

    Serial.println("Reading all Sensors");

    if (magnetDetectedLeft)
    { // if magnet is detected on the right

        digitalWrite(LED, HIGH);

        Serial.println("Magnet detected on the left");

        turn('r');
    }
    else if (magnetDetectedRight)
    { // if magnet is detected on the right

        digitalWrite(15, HIGH);

        Serial.println("Magnet detected on the right");

        turn('l');
    }

    if (lineRight == false && lineLeft == false)
    { // if there's no black line detected

        Serial.println("No line detected");

        if (distanceRight < distanceLeft && distanceLeft < 40 && distanceRight < 40)
        {

            Serial.println("Distance left > distance right");

            turnCustom('l', 125, 80); // if distance to wall is higher on the right, turn a bit to the left

        } else if (distanceRight > distanceLeft && distanceLeft < 40 && distanceRight < 40) {

            Serial.println("Distance left > distance right");

            turnCustom('r', 125, 80); // if distance to wall is higher on the right, turn a bit to the right
        } else {

            Serial.println("No wall on one side");

            drive(100);
        }
    } else if (lineRight == true && lineLeft == false) { // if there's a black line on the right

        Serial.println("Line detected one the right");

        turn('r');

    } else if (lineRight == false && lineLeft == true) { // if there's a black line on the right

        Serial.println("Line detected one the left");

        turn('l');
    }

    if (distanceMiddle < 30)
    {
        Serial.println("Wall in front");

    }

    delay(50);
}