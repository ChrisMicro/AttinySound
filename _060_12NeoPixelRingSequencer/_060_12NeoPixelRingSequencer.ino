/************************************************************************************

 sequencer for 12 LED NeoPixel ring

  This program is an inovative way for fast creation of a lot of 8 bit noise ... 

  hardware:
  
  The full visual effect you will achive when using a 12 LED neo pixel ring instead
  of using only a 8LED neo pixel stripe.

*************************************************************************************

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

********************************** list of outhors **********************************

  v0.1  4.3.2017 C. -H-A-B-E-R-E-R-  initial version

  It is mandatory to keep the list of authors in this code.
  Please add your name if you improve/extend something
  2017 ChrisMicro
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

#define NUMPIXELS      12

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

void setup()
{
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin(); // This initializes the NeoPixel library.

  pinMode(SPEAKERPIN, OUTPUT);
}

// show a triangle on the pixel ring
void triangleIndicator(uint8_t pos, uint32_t color)
{
  pixels.setPixelColor(pos % 12, color);
  pixels.setPixelColor((pos + 4) % 12, color);
  pixels.setPixelColor((pos + 8) % 12, color);
}

// the indicator can be displayed as one dot or as triangle
void showIndicator(uint8_t ledPos, uint8_t indicatorType)
{
  uint8_t r = 0;
  uint8_t b = 0;
  if (indicatorType == 0)
  {
    pixels.setPixelColor(ledPos, pixels.Color(15, 0, 0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    pixels.setPixelColor(ledPos, pixels.Color(0, 0, 0)); // Moderately bright green color.
  } else
  {
    triangleIndicator(ledPos, pixels.Color(0, 15, 0));
    pixels.setPixelColor(ledPos, pixels.Color(15, 0, 0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.
    triangleIndicator(ledPos, 0);
  }
}

uint8_t getKey()
{
  uint8_t key = 0;
  if (analogRead(BUTTONS_ADC) > 600) key = 1;
  if (analogRead(BUTTONS_ADC) < 400) key = 2;
  return key;
}

// scaled the adc of the 8BitMixTape voltage divider
// to a return value in range: 0..1023
uint16_t analogReadScaled(uint8_t channel)
{
  uint16_t value = analogRead(channel);
  if (value > 511) value = 511;
  return value * 2;
}

uint16_t oldPitch;
int8_t   oldLedPos;
uint16_t pitchBuffer[NUMPIXELS];

#define MINTIME 10;

void loop()
{
  uint16_t posPoti;
  uint8_t r = 0;
  uint8_t b = 0;

  uint16_t pitch = analogRead(POTI_RIGHT) + 10;
  int8_t ledPos = 0;

  int16_t inc = 0;

  uint8_t indicatorType = 0;

  showIndicator(ledPos, 0);
  
  uint16_t startTime = millis();
  uint16_t delayTime_ms = 100;
  
  while (1)
  {
    posPoti = analogReadScaled(POTI_LEFT);

    if (posPoti < 300)
    {
      inc = -1;
      delayTime_ms = (posPoti) * 2 + MINTIME;
    }
    else if (posPoti > 700)
    {
      inc = 1;
      delayTime_ms = (1023 - posPoti) * 2 + MINTIME;
    }
    else inc = 0;

    if ( inc == 0 )
    {
      ledPos = ((posPoti - 300) * 11 / 400);
      delayTime_ms = 50;
      indicatorType = 0;
    }
    else
    {
      ledPos += inc;
      indicatorType = 1;
      //delay(100);
    }

    if ( ledPos >= NUMPIXELS ) ledPos = 0;
    if ( ledPos < 0 )          ledPos = NUMPIXELS - 1;

    pitch = analogReadScaled(POTI_RIGHT) + 10;

    if (abs(pitch - oldPitch) > 4)
    {
      pitchBuffer[ledPos] = pitch;
      playSound(pitch, delayTime_ms);
      oldPitch = pitch;
    }

    if (ledPos != oldLedPos)
    {
      showIndicator( ledPos, indicatorType);
      playSound( pitchBuffer[ledPos], delayTime_ms);
      oldLedPos = ledPos;
    }
    
    while ((uint16_t) millis() - startTime < delayTime_ms);
    startTime = millis();
  }

}
