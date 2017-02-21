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


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across ???

*************************************************************************************
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// hardware description / pin connections
#define SPEAKERPIN      1
#define NEOPIXELPIN     0

#define POTI_RIGHT     A1
#define POTI_LEFT      A2
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

void setup()
{
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin(); // This initializes the NeoPixel library.

  pinMode(SPEAKERPIN, OUTPUT);
  //analogReference( INTERNAL2V56 );
  playSound(1000, 100); // beep
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
  if (analogRead(BUTTONS_ADC) < 330) button = 2;
  if (analogRead(BUTTONS_ADC) < 230) button = 3;

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
    pixels.setPixelColor(n, 0); // off
  }
}

void setColorLimitedPixel(uint16_t numberPix, uint32_t color)
{
  uint8_t n;
  for (n = 0; n < numberPix; n++)
  {
    pixels.setPixelColor(n, pixels.Color(color, 0, color)); // off
  }
}

void setColorLimited2Pixel(uint16_t numberPix, uint32_t color)
{
  uint8_t n;
  for (n = NUMPIXELS; n >= NUMPIXELS - numberPix; n--)
  {
    pixels.setPixelColor(n, pixels.Color(0, color, 0)); // off
  }
}

void loop()
{
  pixels.setBrightness(255);
  uint16_t p1 = 500 - analogRead(POTI_LEFT);
  uint16_t p2 = analogRead(POTI_RIGHT);

  setColorAllPixel(0); // pixels off

  setColorLimitedPixel((p1 >> 7), 30);
  setColorLimited2Pixel((p2 >> 7), 50);

  uint8_t x = getButton();
  if (x == 1) pixels.setPixelColor(3, pixels.Color(0, 0, 50));
  if (x == 2) pixels.setPixelColor(4, pixels.Color(30, 30, 0));

  if (x == 3) {
    pixels.setPixelColor(3, pixels.Color(50, 0, 0));
    pixels.setPixelColor(4, pixels.Color(50, 0, 0));
    pixels.show(); // This sends the updated pixel color to the hardware.
    playSound(p1 + 20, (p2/5) + 10);
  }
  
  if (x == 3 && p1 < 30 && p2 < 30) {
    pixels.setBrightness(80);
    rainbowCycle(3, 5);
  }
  
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void rainbowCycle(uint8_t wait, uint8_t rounds) {
  uint16_t i, j;

  for(j=0; j<256*rounds; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
