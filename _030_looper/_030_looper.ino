/*

  sound looper

  This is very experimental ...
  
  8BitMixedTape-SoundProg2085
  Berliner Schule
  Version 0.8 Neo

  button1: hear current pitch
  button2: store note
  both: delete note

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

void setup()
{
  cli();

  // Set up Timer/Counter1 62kHz.
  TCCR1 &= ~(7 << CS10); // make sure bits are empty
  TCCR1 |= 1 << CS10;    // set prescaler

  // pulse width modulator A enable
  TCCR1 &= ~(1 << COM1A0);       // make sure register is empty
  TCCR1 |=   1 << COM1A1 ;       // PWM mode: clear the OC1A output line on compare match

  TCCR1 |= 1 << PWM1A;           // PWM mode

  TIMSK |= 1 << OCIE1A;          // Enable compare match interrupt for timer 1

  OCR1A = 128;                   // initialize PWM to 50% mean value

  DDRB |= 1 << PB1; // PWM pin as output

  sei();
  setFrequency(1000);
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin(); // This initializes the NeoPixel library.

  pinMode(SPEAKERPIN, OUTPUT);
  //analogReference( INTERNAL2V56 );
  //playSound(1000, 100); // beep

}

const int8_t PROGMEM  sintab[] = {
  0 , 3 , 6 , 9 , 12 , 15 , 18 , 21 , 24 , 27 , 30 , 33 , 36 , 39 , 42 , 45 , 48 , 51 , 54 , 57 , 59 , 62 , 65 , 67 , 70 , 73 , 75 , 78 , 80 , 82 , 85 , 87 ,
  89 , 91 , 94 , 96 , 98 , 100 , 102 , 103 , 105 , 107 , 108 , 110 , 112 , 113 , 114 , 116 , 117 , 118 , 119 , 120 , 121 , 122 , 123 , 123 , 124 , 125 , 125 , 126 , 126 , 126 , 126 , 126 ,
  127 , 126 , 126 , 126 , 126 , 126 , 125 , 125 , 124 , 123 , 123 , 122 , 121 , 120 , 119 , 118 , 117 , 116 , 114 , 113 , 112 , 110 , 108 , 107 , 105 , 103 , 102 , 100 , 98 , 96 , 94 , 91 ,
  89 , 87 , 85 , 82 , 80 , 78 , 75 , 73 , 70 , 67 , 65 , 62 , 59 , 57 , 54 , 51 , 48 , 45 , 42 , 39 , 36 , 33 , 30 , 27 , 24 , 21 , 18 , 15 , 12 , 9 , 6 , 3 ,
  0 , -3 , -6 , -9 , -12 , -15 , -18 , -21 , -24 , -27 , -30 , -33 , -36 , -39 , -42 , -45 , -48 , -51 , -54 , -57 , -59 , -62 , -65 , -67 , -70 , -73 , -75 , -78 , -80 , -82 , -85 , -87 ,
  -89 , -91 , -94 , -96 , -98 , -100 , -102 , -103 , -105 , -107 , -108 , -110 , -112 , -113 , -114 , -116 , -117 , -118 , -119 , -120 , -121 , -122 , -123 , -123 , -124 , -125 , -125 , -126 , -126 , -126 , -126 , -126 ,
  -127 , -126 , -126 , -126 , -126 , -126 , -125 , -125 , -124 , -123 , -123 , -122 , -121 , -120 , -119 , -118 , -117 , -116 , -114 , -113 , -112 , -110 , -108 , -107 , -105 , -103 , -102 , -100 , -98 , -96 , -94 , -91 ,
  -89 , -87 , -85 , -82 , -80 , -78 , -75 , -73 , -70 , -67 , -65 , -62 , -59 , -57 , -54 , -51 , -48 , -45 , -42 , -39 , -36 , -33 , -30 , -27 , -24 , -21 , -18 , -15 , -12 , -9 , -6 , -3
};

#define F_CPU 16000000L

volatile uint16_t Freq_coefficient = 1;
volatile uint8_t  Amplitude = 255;

void setFrequency(uint16_t frequency_Hz)
{
  uint16_t temp = (uint32_t) F_CPU / 256 * frequency_Hz / 256 / 128;
  cli();
  Freq_coefficient = temp;
  sei();
}

void setAmplitude(uint8_t amp)
{
  cli();
  Amplitude = amp;
  sei();
}

SIGNAL(TIMER1_COMPA_vect)
{
  static uint16_t phase = 0;
  int16_t temp;

  phase += Freq_coefficient;

  temp  = (int8_t) pgm_read_byte ( &sintab[ phase >> 8 ] ) ;

  temp *= Amplitude;
  temp = (temp >> 8) + 128;
  OCR1A = temp;

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

#define COLOR_RED  0x000F0000
#define COLOR_BLUE 0x0000000F

uint32_t pixelColor(uint32_t colorDefinition)
{
  return pixels.Color(colorDefinition >> 16, colorDefinition >> 8, colorDefinition);
}

// analog read scaled for the 8BitMixTape voltage divider
uint16_t analogReadScaled(uint8_t channel)
{
  uint16_t value = analogRead(channel);
  if (value > 511) value = 511;
  return value * 2;
}

#define LoopTime_ms 5

uint8_t CursorPosition = 0;
uint16_t StartTime_ms;
uint16_t LedTime_ms = 0;

#define  NUMVOICES 4
uint16_t Buffer[NUMPIXELS][NUMVOICES] = {0};
uint8_t Voice = 0;

void loop()
{
  int16_t left  = analogReadScaled( POTI_LEFT  );
  int16_t right = analogReadScaled( POTI_RIGHT );
  static int16_t leftOld = 0;
  uint16_t f_Hz;

  f_Hz = left * 2 + 200;

  leftOld = left;
  // wait for loop cycle
  while ( (uint16_t) millis() - StartTime_ms < LoopTime_ms );
  StartTime_ms = millis();

  LedTime_ms += LoopTime_ms;

  uint16_t noteDuration_ms = right + 50;
  if (LedTime_ms > noteDuration_ms)
  {
    LedTime_ms = 0;

    if (Buffer[CursorPosition][Voice] != 0)
    {
      setFrequency(Buffer[CursorPosition][Voice]);
      setAmplitude(255);
    } else setAmplitude(0);
    setColorAllPixel(0); // pixels off
    uint8_t n;
    for (n = 0; n < NUMPIXELS; n++)
    {
      if (Buffer[n][Voice] != 0)pixels.setPixelColor(n, pixelColor(COLOR_BLUE));
    }
    pixels.setPixelColor(CursorPosition, pixelColor(COLOR_RED));
    pixels.show();

    CursorPosition++;
    if ( CursorPosition >= NUMPIXELS )CursorPosition = 0;

  }
  uint8_t x = getButton();
  if (x == 1) // play
  {    
    setFrequency(f_Hz);
    setAmplitude(255);
    //Buffer[CursorPosition][Voice] = f_Hz;
  }

  if (x == 2) // store
  {    
    Buffer[CursorPosition][Voice] = f_Hz;
  }
  
  if (x == 3) // delete note
  {
    Buffer[CursorPosition][Voice] = 0;
  }

}
