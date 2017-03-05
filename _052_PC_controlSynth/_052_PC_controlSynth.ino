/************************************************************************************

  PC controlled synth

*************************************************************************************

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

********************************** list of outhors **********************************

  v0.1  5.3.2017 C. -H-A-B-E-R-E-R-  initial version

  It is mandatory to keep the list of authors in this code.
  Please add your name if you improve/extend something
  2017 ChrisMicro
*************************************************************************************
*/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "pcControl.h"
#include "neoSoundPittix.h";

// hardware description / pin connections
#define SPEAKERPIN      1
#define NEOPIXELPIN     0

#define NUMPIXELS      8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);


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

void setup()
{
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif

  pinMode(SPEAKERPIN, OUTPUT);
  frameRecieverBegin();

  pixels.begin(); // This initializes the NeoPixel library.
  sei();

  setColorAllPixel(pixels.Color(0, 0, 1)); // pixels off

  pixels.show(); // This sends the updated pixel color to the hardware.

  beginPittix ();
  setFrequency(440); 
  delay(1000);

}

// the command has 5 bits
uint8_t getCommand()
{
  return (FrameData[0]>>2)&0x1f;
}

// the value has 10 bits
uint16_t getValue()
{
  uint16_t value;
  value=FrameData[1];
  value+=(FrameData[0]&0x3) <<8;
  return value;
}

uint8_t LedNumber=0;
uint8_t Red=0;
uint8_t Green=0;
uint8_t Blue=0;
 
void loop()
{
  while(!isPcAvailable());
  stopPittix();
  frameRecieverBegin();
  

  receiveFrame();
  switch(getCommand())
  {
    case 1:
    {
      Red=getValue();
      setFrequency(Red*10);
    }break;
    
    case 2:
    {
      Green=getValue();
      setWaveform(Green/50);

    }break;
    
    case 3:
    {
      Blue=getValue();
      setAmplitude(Blue+1);


    }break;
    
    case 4:
    {
      LedNumber=getValue();
    }break;
    
  }
  
  //playSound(getValue(),100);
  setColorAllPixel(0); // pixels off

  pixels.setPixelColor( LedNumber, pixels.Color(Red, Green, Blue));

  pixels.show(); // This sends the updated pixel color to the hardware.

  beginPittix ();

}
