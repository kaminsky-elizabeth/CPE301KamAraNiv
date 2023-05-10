/*
  Authors: Mikayla Arabian, Evelynne Nivera, and Elizabeth Kaminsky
  Date: 5-3-2023

  Fan and clock code done by Mikayla Arabian
  Stepper motor and water sensor code done by Evelynne Nivera
  LCD and on/off button code by Elizabeth Kaminsky

*/

//Libraries
#include <Stepper.h>
#include <dht.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <SPI.h>

#define STEPS 50
#define RDA 0x80
#define TBE 0x20  
#define DHT11_PIN 7
#define BUTTON PORTD0

//For the state lights
#define WRITE_HIGH_PC(pin_num) *port_c |= (0x01 << pin_num);
#define WRITE_LOW_PC(pin_num) *port_c &= ~(0x01 << pin_num);

//For the fan
#define WRITE_HIGH_PE(pin_num) *port_e |= (0x01 << pin_num);
#define WRITE_LOW_PE(pin_num) *port_e &= ~(0x01 << pin_num);

volatile unsigned char* port_k = (unsigned char*) 0x108; 
volatile unsigned char* ddr_k  = (unsigned char*) 0x107; 
volatile unsigned char* pin_k  = (unsigned char*) 0x106; 

volatile unsigned char* port_c = (unsigned char*) 0x28;
volatile unsigned char* ddr_c = (unsigned char*) 0x27;
volatile unsigned char* pin_c = (unsigned char*) 0x26;

volatile unsigned char* port_e = (unsigned char*) 0x2E; 
volatile unsigned char* ddr_e  = (unsigned char*) 0x2D; 
volatile unsigned char* pin_e  = (unsigned char*) 0x2C; 

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

unsigned int waterThreshold = 250;

//state 0 = disabled, state 1 = idle, state 2 = error, state 3 = running
int state = 0;

Stepper stepper(STEPS, 8, 10, 9, 13);
LiquidCrystal lcd(12, 11, 6, 4, 3, 2);

dht DHT;

//Code for the clock. It messed with the other code so it is commented out.
/*RTC_DS1307 rtc; //RTC_DS1307 class for the clock

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
int numEvents = 0;*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
 
  //set PK2 to INPUT for buttons
  *ddr_k &= 0xFB;
  
  //enables pullup resistor 
  //MAKE SURE THIS IS INCLUDED IN THE FINAL PART 
  *port_k |= 0x04;

  //Lights and fans as INPUT
  *ddr_c |= 0x01;
  *ddr_c |= 0x03;
  *ddr_c |= 0x04;
  *ddr_c |= 0x07;
  *ddr_e |= 0x08;

  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();

  stepper.setSpeed(200);

  lcd.begin(16, 2);

  DDRA &= ~(1<<PORTD0);

  // initialize the real-time clock
 /* rtc.begin();
  if (!rtc.isrunning()){
  Serial.println("RTC is not running!");
  }

  SPI.begin();*/
}

void loop() {

  //If the button is not pushed, the whole thing is disabled
  if (!(PIND & (1 << BUTTON)) == 0)
  {
    WRITE_HIGH_PC(7);
    WRITE_LOW_PC(4);
    WRITE_LOW_PC(3);
    WRITE_LOW_PC(1);

  /*temp = analogRead(sensorPin);
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
  }*/

  //read info from sensor
  int chk = DHT.read11(DHT11_PIN);
  //begin lcd display
  //set display cursor to column 0 line 0
  lcd.setCursor(0,0); 

  WRITE_HIGH_PE(3);

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




  unsigned int data;
  data = adc_read(1);
  //getInt(data);
 // U0getchar();

  if(data<waterThreshold)
  {
    //Puts it into error state
    lcd.clear();
    lcd.print("ERROR: WATER");
    lcd.setCursor(0,1);
    lcd.print("LVL TOO LOW");
    state = 2;
    WRITE_LOW_PC(7);
    WRITE_HIGH_PC(4);
    WRITE_LOW_PC(3);
    WRITE_LOW_PC(1);
  }
  if(data<waterThreshold && DHT.temperature<18)
  {
    state = 1;
    WRITE_LOW_PC(7);
    WRITE_LOW_PC(4);
    WRITE_HIGH_PC(3);
    WRITE_LOW_PC(1);

  }

  //A10 button is pushed, move stepper motor in one direction 
  while(*pin_k & 0x04)
  {
    stepper.step(50);
  }
  //A09 button is pushed, move stepper motor in the other direction 
  while(*pin_k & 0x03)
  {
   stepper.step(-50);
  }

  /*
  if(DHT.temperature<18)
  {
    state = 1;
    WRITE_LOW_PE(3);
  }
  if(DHT.temperature>18 && state!=2)
  {
    state = 3;
    WRITE_HIGH_PE(3);
  }*/
}
  else
  {
    //Disabled state
    state = 0;
    lcd.clear();
    WRITE_LOW_PC(7);
    WRITE_LOW_PC(4);
    WRITE_LOW_PC(3);
    WRITE_HIGH_PC(1);
  }
  delay(1000);
}

void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 6 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11110111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11111000; // clear bit 0-2 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}
unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}

void U0init(int U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
unsigned char U0kbhit()
{
  return *myUCSR0A & RDA;
}
unsigned char U0getchar()
{
  return *myUDR0;
}
void U0putchar(unsigned char U0pdata)
{
  while((*myUCSR0A & TBE)==0);
  *myUDR0 = U0pdata;
}

/*void sendMotorEvents(){
  for(int i = 0; i < numEvents; i++){
    MotorEvent event = motorEvents[i];
    byte data[9];
    data[0] = event.isOn;
  }
}*/

