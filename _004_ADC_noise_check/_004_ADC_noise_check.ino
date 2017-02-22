/*

  Testprogram to check the noise of the ADC
  
  8BitMixedTape-SoundProg2085
  Berliner Schule
  Version 0.8 Neo

  The noise power of the ADC is measured and displayed as binary value on the
  neopixels.

  You can chouse the ADC of the left and the right channel with the buttons.

  On my setup the noise of the ADC is close to zero. 
  When I turn the POTI, there are certain points where the noise goes up to 
  6 LEDs. This are the points where the ADC switches between two digital values.

  If I touch the ADC-Input with the finger, the noise will overshoot and all pixel
  become red.

*************************************************************************************

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

********************************** list of outhors **********************************

  v0.1  22.2.2017 C. -H-A-B-E-R-E-R-  initial version

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

#define NEOPIXELPIN     0

#define POTI_LEFT      A1
#define POTI_RIGHT     A2
#define BUTTONS_ADC    A3

#define NUMPIXELS      8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);



void setup()
{
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin(); // This initializes the NeoPixel library.

  //analogReference( INTERNAL2V56 );

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
  }
}

void setColorAllPixel(uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    pixels.setPixelColor(n, color); // off
  }
}

#define NUMBER_OF_SAMPLES 100
int16_t Values[NUMBER_OF_SAMPLES];

void loop()
{
  uint16_t n;
  static uint8_t adcChannelSelector=0;
  
  float meanValue=0;

  for(n=0;n<NUMBER_OF_SAMPLES;n++)
  {
    uint16_t p1;

    uint16_t value;
    if(adcChannelSelector==0) value=analogRead(POTI_LEFT);
    else value = analogRead(POTI_RIGHT);
    
    Values[n]=value;
    meanValue+=value;
  }
  meanValue = meanValue/NUMBER_OF_SAMPLES;

  float noisePower=0;
  for( n=0; n<NUMBER_OF_SAMPLES; n++)
  {
    noisePower+=(Values[n]-meanValue)*(Values[n]-meanValue);
  }

  setColorAllPixel(0); // pixels off
  
  uint32_t color;
  if( adcChannelSelector==0 ) color = pixels.Color(0, 15, 0);
  else                        color = pixels.Color(0, 0, 15);
  
  if( noisePower < 256 )
  {
    displayBinrayValue( noisePower, color);
  }else // overflow: make all pixels red
  { 
    setColorAllPixel(pixels.Color(15, 0, 0)); // pixels off
  }

  pixels.show(); // This sends the updated pixel color to the hardware.

  uint8_t x = getButton();
  if (x & 1) adcChannelSelector=0;
  if (x & 2) adcChannelSelector=1;

}
