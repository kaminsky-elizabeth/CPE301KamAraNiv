//LCD display

#include <dht.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

dht DHT;

#define DHT11_PIN 7

void setup(){
  lcd.begin(16, 2);
}

void loop(){

  //read info from sensor
  int chk = DHT.read11(DHT11_PIN);

  //begin lcd display
  //set display cursor to column 0 line 0
  lcd.setCursor(0,0); 

  //display temp
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");

  //set display cursor to column 0 line 1
  lcd.setCursor(0,1);

  //display humidity
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");

  //delay before looping
  delay(1000);
}