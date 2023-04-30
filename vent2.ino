/*
Stepper Motor Vent Prototype
Written by Evie Nivera

Code for the stepper motor vent thing. It is based on lab 3. Angle of the stepper motor is controlled with two buttons. 
*/

#include <Stepper.h>

//Stuff from lab 3 I am keeping these right now for testing using LED
#define WRITE_HIGH_PD(pin_num) *port_d |= (0x01 << pin_num);
#define WRITE_LOW_PD(pin_num) *port_d &= ~(0x01 << pin_num);
#define WRITE_HIGH_PE(pin_num) *port_e |= (0x01 << pin_num);
#define WRITE_LOW_PE(pin_num) *port_e &= ~(0x01 << pin_num);


//Port K is the only one that will be necessary for the final version but I'm leaving the others for testing. 
//Maybe we can use d and e for two of the state lights? Unless we are already using those pins but I'm not sure. 
volatile unsigned char* port_k = (unsigned char*) 0x108; 
volatile unsigned char* ddr_k  = (unsigned char*) 0x107; 
volatile unsigned char* pin_k  = (unsigned char*) 0x106; 

volatile unsigned char* port_d = (unsigned char*) 0x2B; 
volatile unsigned char* ddr_d  = (unsigned char*) 0x2A; 
volatile unsigned char* pin_d  = (unsigned char*) 0x29; 

volatile unsigned char* port_e = (unsigned char*) 0x2E; 
volatile unsigned char* ddr_e  = (unsigned char*) 0x2D; 
volatile unsigned char* pin_e  = (unsigned char*) 0x2C; 

#define STEPS 32


//Stepper

Stepper stepper(STEPS, 8, 10, 9, 11);

void setup() {
  Serial.begin(9600);
 
  //set PK2 to INPUT
  *ddr_k &= 0xFB;
  
  //enables pullup resistor 
  //MAKE SURE THIS IS INCLUDED IN THE FINAL PART 
  *port_k |= 0x04;

  //sets PD0 and PE3 to the outputs 
  *ddr_d |= 0x01;
  *ddr_e |= 0x08;
  
  stepper.setSpeed(200);

  //I got lazy so I'm using pinmode/digitalWrite to test the third light this will not be in the final version
  pinMode(12, OUTPUT);
}

void loop() {

  //A10 button is pushed, move stepper in one direction 
  if(*pin_k & 0x04)
  {
    WRITE_HIGH_PD(0);
    WRITE_LOW_PE(3);
    stepper.step(20);
    
  }

  /*else{
    WRITE_LOW_PD(0);
    WRITE_HIGH_PE(3);
   // stepper.step(0);
  }*/

  //A09 button is pushed, move stepper in the other direction 
  if(*pin_k & 0x03)
  {
    stepper.step(-20);
    digitalWrite(12, HIGH);
    digitalWrite(5, LOW);
  }
  else
  {
   /* stepper.step(0);
    digitalWrite(12, LOW);
       WRITE_HIGH_PE(3);*/

    WRITE_LOW_PD(0);
    digitalWrite(12, LOW);
    WRITE_HIGH_PE(3);
  }

}
