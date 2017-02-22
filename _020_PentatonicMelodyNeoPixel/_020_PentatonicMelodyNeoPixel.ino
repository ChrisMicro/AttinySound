/*
   Arduino meditave music

   pentatonic music like played on a hang drum
   https://en.wikipedia.org/wiki/Hang_(instrument)

   original version: Jeremy Fonte
   addapted and corrected for attiny85
   2017 ChrisMicro

   Hardware Platform: Attiny85

*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            0

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 100; // delay for half a second


#define SPEAKERPIN 1

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
  //MotorSpeed(255, 255);

  for (n = 0; n < counts; n++)
  {
    //digitalWrite(SPEAKERPIN,0);
    PINHIGH;
    delayMicroseconds(delayTime_us);
    PINLOW;
    //digitalWrite(SPEAKERPIN,1);
    delayMicroseconds(delayTime_us);
  }
}

// http://fonte.me/arduino/#Musician
/*
  Musician
  Plays a (fairly) random tune until the program is stopped
  8-ohm speaker on digital pin 8
  //Copyright (c) 2012 Jeremy Fonte
  //This code is released under the MIT license
  //http://opensource.org/licenses/MIT
*/

int randomNote = 131;
int randomDuration = 2;
int noteStep = 1;
int notes[16];


void setup()
{
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.

  notes[1] = 131;
  notes[2] = 147;
  notes[3] = 165;
  notes[4] = 196;
  notes[5] = 220;
  notes[6] = 262;
  notes[7] = 294;
  notes[8] = 330;
  notes[9] = 392;
  notes[10] = 440;
  notes[11] = 523;
  notes[12] = 587;
  notes[13] = 659;
  notes[14] = 784;
  notes[15] = 880;

  randomNote = random(1, 15);

  pinMode(SPEAKERPIN, OUTPUT);
}

void loop() {

  static uint8_t count = 0;
  static uint8_t oldPixelNumber = 0;

  count++;

  noteStep = random(-3, 3);
  randomNote = randomNote + noteStep;
  if (randomNote < 1) {
    randomNote = random(1, 15);
  }
  else if (randomNote > 15) {
    randomNote = random(1, 15);
  }

  randomDuration = random(1, 8);
  // to calculate the note duration, take one second
  // divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  int noteDuration = 300 / randomDuration;

  playSound( notes[randomNote], noteDuration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  int pauseBetweenNotes = noteDuration * 1.30;
  delay(10);
  
  uint8_t pixelNumber = randomNote & 0x07;

  pixels.setPixelColor(oldPixelNumber, pixels.Color(0, random(0,4), 0)); // Moderately bright green color.
  oldPixelNumber = pixelNumber;
  pixels.show(); // This sends the updated pixel color to the hardware.
  pixels.setPixelColor(pixelNumber, pixels.Color(randomNote, random(0,randomNote/2), random(0,15))); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}





