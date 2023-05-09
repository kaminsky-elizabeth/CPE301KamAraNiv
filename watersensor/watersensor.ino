//
// WATER SENSOR
// NOTE: The exact water level tends to be a bit scuffed immediately after removing it from water it should even out eventually.
//Evelynne Nivera
#define RDA 0x80
#define TBE 0x20  

//There are 4 states. For now I have set these up as 0 = ERROR, 1 = DISABLED, 2 = IDLE, 3 = RUNNING. 

unsigned int threshold = 250;

int state;

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

void setup() 
{
  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();

}
void loop() 
{
  if(state!=1)
  {
    // do loopy stuff
    unsigned int data;
    data = adc_read(1);
    getInt(data);
    U0getchar();

  //Error state is based on the water level.
  //As stated above, the digitalwrite functions WILL BE DELETED they are only here to make it easy to check state
  //The water sensor goes off if state is DISABLED. I'm not sure how I want to go about turning it off at the moment maybe 
  //we can discuss this next week. 
  if (data<threshold)
  {
    //Sets state to ERROR if water level is below threshold. 
    state = 0;
    if(state==0)
    {
  
    }
  }
  else
  {
    //Sets state to IDLE if water level is above threshold. 
    state = 2;

  }
  
  }
  
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

//This converts the data to char, so that it can be displayed on the serial plotter. I'm not sure if we'll need this in the end or
// not but it is usefull for checking the values. It works by splicing up the input data into 1s, 10s, 100s, 1000s position. 
void getInt(unsigned int data)
{
  unsigned int flag = 0;

  if(data >=1000)
  {
    U0putchar ((data/1000) + '0');
    data = data % 1000;
    flag = 1;
  }

  if(data >=100 || flag ==1)
  {
    U0putchar((data/100) + '0');
    data = data % 100;
    flag = 1;
  }

  if(data >=10 || flag ==1)
  {
    U0putchar((data/10) + '0');
    data = data % 10;
    flag =1;
  }

  U0putchar(data + '0');
  U0putchar('\n');
}
