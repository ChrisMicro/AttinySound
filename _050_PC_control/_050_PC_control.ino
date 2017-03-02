/************************************************************************************

  receiver program to receive to bytes from the audio line

  With this program you can remote control your microcontroller over the audio line.
  The first byte controls the LEDs.
  The second byte controls the beeper.

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
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "pcControl.h"

// hardware description / pin connections
#define SPEAKERPIN      1
#define NEOPIXELPIN     0

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
  playSound(1000, 100); // beep
  delay(3000);

  pixels.begin(); // This initializes the NeoPixel library.
  setColorAllPixel(pixels.Color(0, 0, 1)); // pixels off

  pixels.show(); // This sends the updated pixel color to the hardware.
}

void loop()
{

  receiveFrame();
  playSound(FrameData[1],100);
  setColorAllPixel(0); // pixels off

  displayBinrayValue( FrameData[0], pixels.Color(0, 15, 0));

  //displayBinrayValue( Counter++, pixels.Color(0, 15, 0));

  pixels.show(); // This sends the updated pixel color to the hardware.


}
