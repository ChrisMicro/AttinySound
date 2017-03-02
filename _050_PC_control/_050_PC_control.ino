/*

  Testprogram for the hardware of the 8Bit synthesizer

  8BitMixedTape-SoundProg2085
  Berliner Schule
  Version 0.8 Neo

  With this software you can test if your hardware is working correct.
  The speacker makes a beep sound.
  The value of the potientiometer is displayed as binary value on the leds.
  One potientiometer is displayed in green and the other one is displayed in blue.
  You can press the buttons. They are displayed in red. If both buttons are pressed,
  it will make a sound.

*************************************************************************************

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

********************************** list of outhors **********************************

  v0.1  15.2.2017 C. -H-A-B-E-R-E-R-  initial version

  It is mandatory to keep the list of authors in this code.
  Please add your name if you improve/extend something
   2017 ChrisMicro

*************************************************************************************

  Hardware Platform: Attiny85

                                      ATTINY85 Pins
                                      =============

                                             _______
                                            |   U   |
  DEBUGLED / SYNC-OUT             reset/PB5-|       |- VCC
  soundprog / buttons ->  D3/A3         PB3-| ATTINY|- PB2    D2/A1  <- POTI1 / CV-IN
  POTI2 / SYNC-IN ->      D4/A2         PB4-|   85  |- PB1    D1     -> PWM SOUND
                                        GND-|       |- PB0    D0     -> NEOPIXELS
                                            |_______|


*************************************************************************************
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// hardware description / pin connections
#define SPEAKERPIN      1
#define NEOPIXELPIN     0

#define POTI_LEFT      A1
#define POTI_RIGHT     A2
#define BUTTONS_ADC    A3

#define NUMPIXELS      8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

// fast pin access
#define AUDIOPIN (1<<SPEAKERPIN)
#define PINLOW (PORTB&=~AUDIOPIN)
#define PINHIGH (PORTB|=AUDIOPIN)

void playSound(long freq_Hz, long duration_ms)
{
  uint16_t n;
  uint32_t delayTime_us;
  uint32_t counts;

  delayTime_us = 1000000UL / freq_Hz / 2;
  counts = duration_ms * 1000 / delayTime_us;

  for (n = 0; n < counts; n++)
  {
    PINHIGH;
    delayMicroseconds(delayTime_us);
    PINLOW;
    delayMicroseconds(delayTime_us);
  }
}




/*
  uint8_t getButton()

  return value:
  1: left button pressed
  2: right button pressed
  3: both buttons pressed
  ADC values
  no button:512, left:341, right:248, both:200
*/
uint8_t getButton()
{
  uint8_t button = 0;
  if (analogRead(BUTTONS_ADC) < 450) button = 1;
  if (analogRead(BUTTONS_ADC) < 300) button = 2;
  if (analogRead(BUTTONS_ADC) < 220) button = 3;

  return button;
}

void displayBinrayValue(uint16_t value, uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    if (value & (1 << n)) pixels.setPixelColor(n, color);
    //else pixels.setPixelColor(n,0); // off
  }
}

void setColorAllPixel(uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    pixels.setPixelColor(n, color);
  }
}

#ifdef DEBUGON

#define DEBUGPIN       ( 1<<PB2 )
#define INITDEBUGPIN   { DDRB  |=  DEBUGPIN; }

#define DEBUGPINON     { PORTB |=  DEBUGPIN;}
#define DEBUGPINOFF    { PORTB &= ~DEBUGPIN;}
#define TOGGLEDEBUGPIN { PORTB ^=  DEBUGPIN;}

#else

#define DEBUGPIN
#define INITDEBUGPIN

#define DEBUGPINON
#define DEBUGPINOFF
#define TOGGLEDEBUGPIN

#endif


#define USELED
#ifdef USELED

#define LEDPORT    ( 1<<PB1 ); //PB1 pin 6 Attiny85
#define INITLED    { DDRB|=LEDPORT; }

#define LEDON      { PORTB|=LEDPORT;}
#define LEDOFF     { PORTB&=~LEDPORT;}
#define TOGGLELED  { PORTB^=LEDPORT;}

#else

#define LEDPORT
#define INITLED

#define LEDON
#define LEDOFF
#define TOGGLELED

#endif

#define INPUTAUDIOPIN (1<<PB3) //
#define PINVALUE (PINB&INPUTAUDIOPIN)
#define INITAUDIOPORT {DDRB&=~INPUTAUDIOPIN;} // audio pin is input

#define PINLOW (PINVALUE==0)
#define PINHIGH (!PINLOW)

#define WAITBLINKTIME 10000
#define BOOT_TIMEOUT  10

#define true (1==1)
#define false !true

#define TIMER TCNT1 // we use timer1 for measuring time


#define FRAMESIZE       2



uint8_t FrameData[ FRAMESIZE ];

//***************************************************************************************
// receiveFrame()
//
// This routine receives a differential manchester coded signal at the input pin.
// The routine waits for a toggling voltage level.
// It automatically detects the transmission speed.
//
// output:    uint8_t flag:     true: checksum OK
//            uint8_t FramData: global data buffer
//
//***************************************************************************************
volatile uint16_t delayTime;
uint8_t receiveFrame()
{
 // cli();
  //uint16_t store[16];

  uint16_t counter = 0;
  volatile uint16_t time_ = 0;

  uint8_t p, t;
  uint8_t k = 8;
  uint8_t dataPointer = 0;
  uint16_t n;

  //*** synchronisation and bit rate estimation **************************
  time_ = 0;
  // wait for edge
  p = PINVALUE;
  while (p == PINVALUE);

  p = PINVALUE;

  TIMER = 0; // reset timer

  for (n = 0; n < 16; n++)
  {
    // wait for edge
    while (p == PINVALUE);
    t = TIMER;
    TIMER = 0; // reset timer
    p = PINVALUE;

    //store[counter++] = t;

    if (n >= 8)time_ += t; // time accumulator for mean period calculation only the last 8 times are used
  }

  delayTime = time_ * 3 / 4 / 8;
  if ( delayTime < 60 ) delayTime = 60;
  // delay 3/4 bit
  while (TIMER < delayTime);

  //****************** wait for start bit ***************************
  while (p == PINVALUE) // while not startbit ( no change of pinValue means 0 bit )
  {
    // wait for edge
    while (p == PINVALUE);
    p = PINVALUE;
    TIMER = 0;

    // delay 3/4 bit
    while (TIMER < delayTime);
    TIMER = 0;
    t = PINVALUE;

    counter++;
  }

  p = PINVALUE;
  TOGGLELED;
  //****************************************************************
  //receive data bits
  k = 8;
  for (n = 0; n < (FRAMESIZE * 8); n++)
  {
    counter++;

    FrameData[dataPointer] = FrameData[dataPointer] << 1;
    if (p != t) FrameData[dataPointer] |= 1;
    p = t;
    k--;
    if (k == 0)
    {
      dataPointer++;
      k = 8;
      if (FrameData[0] != 0xF) return false;

    }
    // wait for edge
    while (p == PINVALUE);
    TIMER = 0;
    p = PINVALUE;

    TOGGLELED;
    // delay 3/4 bit
    while (TIMER < delayTime);

    t = PINVALUE;
    TOGGLELED;
    //TOGGLELED;



  }
  TOGGLELED;
  //if(FrameData[0]!=0xF) return false;
  return true;
 // sei();
}

void  frameRecieverBegin()
{
  INITDEBUGPIN
  INITLED;
  INITAUDIOPORT;

  // Timer 0 normal mode, clk/8, count up from 0 to 255
  // ==> frequency @16MHz= 16MHz/8/256=7812.5Hz
  //TCCR0B = _BV(CS01);
  TCCR1 &= ~(7 << CS10); // make sure bits are empty
  TCCR1 |= 1 << CS12 | 1 << CS10;    // set prescaler
  PLLCSR &=~ 1<<PCKE; // slow clock
  //PLLCSR &=~ 1<<PCKE;
}
void delay_ms(uint32_t ms)
{
  uint32_t n;
  for(n=0;n<ms*10;n++)
  {   
    TIMER=0;
    while(TIMER<200);// 10kHz
  }
}
void setup()
{

#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif



  pinMode(SPEAKERPIN, OUTPUT);
  frameRecieverBegin();
  playSound(1000, 100); // beep
  delay(3000);

  pixels.begin(); // This initializes the NeoPixel library.
  setColorAllPixel(pixels.Color(0, 0, 1)); // pixels off

  pixels.show(); // This sends the updated pixel color to the hardware.
}
int Counter = 0;
void loop()
{
/*
while(1)

{
  TOGGLELED;
  delay_ms(1000);
}
*/

  if (!receiveFrame())
  {
    delay_ms(123);
  }
  else
  {
    //TOGGLELED;
    delay_ms(10);
  }
  //receiveFrame();
  //receiveFrame();

  //TOGGLELED;

  setColorAllPixel(0); // pixels off

  displayBinrayValue( FrameData[1], pixels.Color(0, 15, 0));
  //displayBinrayValue( FrameData[CRCLOW], pixels.Color(0, 15, 0));
  //displayBinrayValue( Counter++, pixels.Color(0, 15, 0));

  //displayBinrayValue( delayTime, pixels.Color(0, 15, 0));

  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(300);
  // wait for edge
  //uint8_t p, t;
  //p = PINVALUE;
  //while (p == PINVALUE);
  //playSound(1000, 100); // beep
  /*
    uint16_t p1 = analogRead(POTI_LEFT);
    uint16_t p2 = analogRead(POTI_RIGHT);

    setColorAllPixel(0); // pixels off

    displayBinrayValue( p1 / 4, pixels.Color(0, 15, 0));
    displayBinrayValue( p2 / 4, pixels.Color(0, 0, 15));

    //pixels.setPixelColor(0, 0); // off
    //pixels.setPixelColor(1, 0); // off

    uint8_t x = getButton();
    if (x & 1) pixels.setPixelColor(0, pixels.Color(15, 0, 0));
    if (x & 2) pixels.setPixelColor(1, pixels.Color(15, 0, 0));
    pixels.show(); // This sends the updated pixel color to the hardware.

    if (x == 3)playSound(p1 + 300, 100);
  */
}
