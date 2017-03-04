#include "neoSoundPittix.h";

void setup()
{
  beginNeoPittix ();
  setFrequency(440); 
  delay(1000);
}

#define POTI_LEFT      A1
#define POTI_RIGHT     A2

void loop()
{
  setFrequency( analogRead(POTI_RIGHT)*5 );
  setWaveform ( analogRead(POTI_LEFT)/100);
  delay(500);
  
  setFrequency(0); // off
  delay(500);

}


