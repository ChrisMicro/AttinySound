/************************************************************************************
  receiver program to receive to bytes from the audio line
*************************************************************************************
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
********************************** list of outhors **********************************
  v0.1  2.3.2017 C. -H-A-B-E-R-E-R-  initial version
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
#include "pcControl.h"

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
      //if (FrameData[0] != 0xF) return false;

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

}

/*
void delay_ms(uint32_t ms)
{
  uint32_t n;
  for(n=0;n<ms*10;n++)
  {   
    TIMER=0;
    while(TIMER<200);// 10kHz
  }
}*/
