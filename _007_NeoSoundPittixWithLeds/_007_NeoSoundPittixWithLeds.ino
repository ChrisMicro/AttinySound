#include "neoSoundPittix.h";

#define POTI_LEFT      A1
#define POTI_RIGHT     A2

#define NUMPIXELS      8
#define NEOPIXELPIN     0

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

void setColorAllPixel(uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    pixels.setPixelColor(n, 0); // off
  }
}

void setup()
{
  beginNeoPittix ();
  setFrequency(440);
  delay(1000);
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin(); // This initializes the NeoPixel library.

}

uint8_t Counter = 1;
void loop()
{
  setFrequency( analogRead(POTI_RIGHT) * 5 );
  setWaveform ( analogRead(POTI_LEFT) / 100);
  delay(500);

  setFrequency(0); // off
  delay(500);

  setColorAllPixel(1);
  pixels.setPixelColor(Counter++ & 0x7, pixels.Color(0, 15, 0));
  pixels.show(); // This sends the updated pixel color to the hardware.

}


