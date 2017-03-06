/*
 * 
 * simple blink program with macros
 * 
 * 6.3.2017 ChrisMicro
 * 
 */
 
#define LEDPORT    ( 1<<PB1 ); //PB1 is the led pin ( pin 6 Attiny85 )
#define INITLED    { DDRB|=LEDPORT; }

#define LEDON      { PORTB|=LEDPORT;}
#define LEDOFF     { PORTB&=~LEDPORT;}
#define TOGGLELED  { PORTB^=LEDPORT;}


void setup() 
{
  INITLED;
}


void loop() 
{
  TOGGLELED;
  delay(1000); // wait for a second
}
