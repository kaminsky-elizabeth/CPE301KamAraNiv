//#include "MegunoLink.h"
int relayPin = 11;

//TimePlot MyPlot;
void setup()
{
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  Serial.println("Analog Sensor Plotter");
}

void loop()
{
  digitalWrite(relayPin, HIGH);
  runL();
  Serial.println(" LED OFF");
  digitalWrite(relayPin, LOW);
  delay(1000);      // added to show off time
}

void runL()
{
  for ( int x = 0; x < 10; x++)
  {

    //float SensorValue = analogRead(3);
    //float voltage = SensorValue * (5.0 / 1023.0);
    //MyPlot.SendData("My Sensor", voltage);
    Serial.print(" x = ");
    Serial.print(x);
    Serial.println("LED ON");
    delay(1000);
  }

}