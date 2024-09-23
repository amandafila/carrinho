#include <QTRSensors.h>

#define TIMEOUT 2500 
#define NUM_SENSORS 8 

QTRSensors qtr;

int IndicatorLED = 13;

//Motor 1
int pinAIN1 = 5; //Direction
int pinAIN2 = 7; //Direction
int pinPWMA = 10; //Speed

//Motor 2
int pinBIN1 = 6; 
int pinBIN2 = 9; 
int pinPWMB = 11; 

int pinSTBY = 8;

unsigned int sensorValues [NUM_SENSORS];
unsigned int sensors [8];

int position = 0;
int error = 0;
int m1Speed = 0;
int m2Speed = 0;
int motorSpeed = 0;
unsigned long previousMillis = 0; 
int BlinkTime = 300; 
int CycleTime = 1; 

int lastError = 0;
float KP = 0.9;
float KD = 0;
int M1 = 110; 
int M2 = 110; 
int M1max = 110; 
int M2max = 110; 
int M1min = 0; 
int M2min = 0; 


static boolean turnCW = 0; //for motorDrive function
static boolean turnCCW = 1; //for motorDrive function
static boolean motor1 = 0; //for motorDrive, motorStop, motorBrake functions
static boolean motor2 = 1; //for motorDrive, motorStop, motorBrake functions

void setup()
{


pinMode(IndicatorLED, OUTPUT);
digitalWrite(IndicatorLED, HIGH);
pinMode(pinPWMA, OUTPUT);
pinMode(pinAIN1, OUTPUT);
pinMode(pinAIN2, OUTPUT);
pinMode(pinPWMB, OUTPUT);
pinMode(pinBIN1, OUTPUT);
pinMode(pinBIN2, OUTPUT);
pinMode(pinSTBY, OUTPUT);


  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A5, A4, A3, A2, A1}, NUM_SENSORS);
  qtr.setEmitterPin(2);
  
 for (uint16_t i = 0; i < 400; i++)
  {
    TimeCheck();
    delay (5);
    qtr.calibrate();
    }

for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
  }

for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
  }

for (int i = 0; i < 5; i++) // make the LED blink for about 5 seconds
{
TimeCheck();
delay(1000);
}
}

void loop()
{
TimeCheck();

position = qtr.readLineWhite(sensors);

error = position - 3500;

motorSpeed = KP * error + KD * (error - lastError);
lastError = error;

m1Speed = M1 - motorSpeed; 
m2Speed = M2 + motorSpeed;

if (m1Speed < M1min) 
m1Speed = M1min;
if (m2Speed < M2min) m2Speed = M2min; if (m1Speed > M1max) //maximum allowed value
m1Speed = M1max;
if (m2Speed > M2max) 
m2Speed = M2max;
// set motor speeds using the two motor speed variables above 255

forward(); 
}

void forward()
{
TimeCheck();
motorDrive(motor1, turnCW, m1Speed);
motorDrive(motor2, turnCW, m2Speed);
}

void TimeCheck() {
unsigned long currentMillis = millis();
if (currentMillis - previousMillis > BlinkTime) {
// save the last time you blinked the LED
previousMillis = currentMillis;

BlinkIt();
}
}

void motorDrive(boolean motorNumber, boolean motorDirection, int motorSpeed)
{
boolean pinIn1; //Relates to AIN1 or BIN1 (depending on the motor number specified)

//Specify the Direction to turn the motor
//Clockwise: AIN1/BIN1 = HIGH and AIN2/BIN2 = LOW
//Counter-Clockwise: AIN1/BIN1 = LOW and AIN2/BIN2 = HIGH
if (motorDirection == turnCW)
pinIn1 = HIGH;
else
pinIn1 = LOW;

//Select the motor to turn, and set the direction and the speed
if (motorNumber == motor1)
{
digitalWrite(pinAIN1, pinIn1);
digitalWrite(pinAIN2, !pinIn1); //This is the opposite of the AIN1
analogWrite(pinPWMA, motorSpeed);
}
else
{
digitalWrite(pinBIN1, pinIn1);
digitalWrite(pinBIN2, !pinIn1); //This is the opposite of the BIN1
analogWrite(pinPWMB, motorSpeed);
}

//Finally , make sure STBY is disabled - pull it HIGH
digitalWrite(pinSTBY, HIGH);
}

void motorBrake(boolean motorNumber)
{
/*
This "Short Brake"s the specified motor, by setting speed to zero
*/

if (motorNumber == motor1)
analogWrite(pinPWMA, 0);
else
analogWrite(pinPWMB, 0);
}

void motorStop(boolean motorNumber)
{
/*
This stops the specified motor by setting both IN pins to LOW
*/
if (motorNumber == motor1) {
digitalWrite(pinAIN1, LOW);
digitalWrite(pinAIN2, LOW);
}
else
{
digitalWrite(pinBIN1, LOW);
digitalWrite(pinBIN2, LOW);
}
}

void BlinkIt() {
if (BlinkCycle == 1) {
//Led On
digitalWrite(IndicatorLED, HIGH);
}
if (BlinkCycle == 2) {
//Led Off
digitalWrite(IndicatorLED, LOW);
BlinkCycle = 0; // reset BlinkCycle
}
BlinkCycle++; // Set BlinkCycle to second mode
}
