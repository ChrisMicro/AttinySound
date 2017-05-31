/*
  simple instrument

  Use on potientiomter to choose the pitch an press a button for the sound.

  31.5.2017 ChrisMicro
*/

#include "neolib.h"
#include "neoSoundPittix.h"
#include "Pt1.h"


#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047

uint16_t notes[]=
{
NOTE_C5, 
NOTE_D5, 
NOTE_E5,  
NOTE_F5, 
NOTE_G5,  
NOTE_A5, 
NOTE_B5,
NOTE_C6
};

void setPixel(uint8_t led, uint32_t color)
{
  pixels.setPixelColor( led, color );
  pixels.show();
}

void setup()
{
  neobegin();
  setPixel(1, COLOR_PINK);
  delay(1000);
  beginNeoPittix();
}

uint8_t  LedPosition   = 0;

#define POITHYSTERESIS 15

int16_t oldPotiLeft;
int16_t oldPotiRight;

Pt1 lp1(0x200);

void loop()
{
  uint8_t b = getButton();

  if ( b == BUTTON_LEFT )
  {
    setWaveform( SINUS );
    setFrequency(notes[LedPosition]);
    lp1.setOutput(255);
  }
  if ( b == BUTTON_RIGHT )
  {
    setWaveformFine( getPoti( POTI_RIGHT) );
    setFrequency(notes[LedPosition]);
    lp1.setOutput(255);
  }
  //if ( b == BUTTON_LEFT + BUTTON_RIGHT )   setColorAllPixel( COLOR_PINK );

  int16_t lp = getPoti( POTI_LEFT );   // range 0..1023
  
  if ( abs(lp - oldPotiLeft) > POITHYSTERESIS )
  {
    oldPotiLeft = lp;
    LedPosition = lp / 32;
    if (LedPosition > 7)LedPosition = 7;
    LedPosition = 7 - LedPosition;

    setColorAllPixel( COLOR_BLACK );
    setPixel(LedPosition, COLOR_GREEN );
  }

  setAmplitude( lp1.filter(0) );
  delay(2);

}
