/*
  2nd Code part for CPE 301 Final Project
  Stop and start recording
*/

#include <RTClib.h>
#include <SPI.h>

RTC_DS1307 rtc; //RTC_DS1307 class for the clock

const int sensorPin = 7; //analog input pin for temperature sensor
const int fanPin = 6; //PWM output pin for fan motor

int temp;
int fanSpeed;
bool motorOn = false;

struct MotorEvent{
  bool isOn;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t day;
  uint8_t month;
  uint16_t year;
};

const int MAX_EVENTS = 100;
MotorEvent motorEvents[MAX_EVENTS];
int numEvents = 0;

void setup(){
  pinMode(fanPin, OUTPUT); // set fan pin as output
  
  // initialize the real-time clock
  rtc.begin();
  if (!rtc.isrunning()){
    Serial.println("RTC is not running!");
  }

  SPI.begin();
}

void loop(){
  temp = analogRead(sensorPin);
  temp = (5.0 * temp * 100.0) / 1024.0; //converting analog value to temperature in Celsius
  
  if(temp > 30 && !motorOn){
    fanSpeed = map(temp, 30, 50, 0, 255);
    analogWrite(fanPin, fanSpeed);
    
    // record the time and date the motor was turned on
    DateTime now = rtc.now();
    MotorEvent event = {true, now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year()};
    motorEvents[numEvents++] = event;
    
    motorOn = true;
  } 
  else if(temp < 20 && motorOn){
    analogWrite(fanPin, 0);
    
    //record the time and date the motor was turned off
    DateTime now = rtc.now();
    MotorEvent event = {false, now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year()};
    motorEvents[numEvents++] = event;
    
    motorOn = false;
  }
  
  delay(1000);
}

void sendMotorEvents(){
  for(int i = 0; i < numEvents; i++){
    MotorEvent event = motorEvents[i];
    byte data[9];
    data[0] = event.isOn;
  }
}
