/*
  1st Code part for CPE 301 Final Project
  Start and stop the fan based on temperature
*/

const int sensorPin = 7;
const int fanPin = 6;

int temp;
int fanSpeed;

void setup(){
  pinMode(fanPin, OUTPUT);
}

void loop(){
  temp = analogRead(sensorPin);
  temp = (5.0 * temp * 100.0) / 1024.0; //converting analog value to temperature in Celsius
  
  if(temp > 30){
    fanSpeed = map(temp, 30, 50, 0, 255); 
    analogWrite(fanPin, fanSpeed);
  }
  else if (temp < 20){
    analogWrite(fanPin, 0);
  }
  
  delay(1000);
}
