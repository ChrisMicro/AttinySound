/*
  
  ATTINY PWM-DDS Waveformgenerator for PB1 output

  Frequency Modulation


  ************************************************************************
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  ********************* list of outhors **********************************
   
  v0.1  15.2.2017 C. -H-A-B-E-R-E-R-  initial version

  It is mandatory to keep the list of authors in this code.
  Please add your name if you improve/extend something

  
*/

void setup() {
  
  cli();

  // Set up Timer/Counter1 62kHz.
  TCCR1 &= ~(7<<CS10);   // make sure bits are empty
  TCCR1 |= 1<<CS10;      // set prescaler

  // pulse width modulator A enable
  TCCR1 &= ~(1 << COM1A0);       // make sure register is empty
  TCCR1 |=   1 << COM1A1 ;       // PWM mode: clear the OC1A output line on compare match

  TCCR1 |= 1 << PWM1A;           // PWM mode

  TIMSK |= 1 << OCIE1A;          // Enable compare match interrupt for timer 1

  OCR1A = 128;                   // initialize PWM to 50% mean value

  DDRB|= 1<<PB1; // PWM pin as output

  sei();
  setFrequency(1000);
}

const int8_t PROGMEM  sintab[]={
0 ,3 ,6 ,9 ,12 ,15 ,18 ,21 ,24 ,27 ,30 ,33 ,36 ,39 ,42 ,45 ,48 ,51 ,54 ,57 ,59 ,62 ,65 ,67 ,70 ,73 ,75 ,78 ,80 ,82 ,85 ,87 ,
89 ,91 ,94 ,96 ,98 ,100 ,102 ,103 ,105 ,107 ,108 ,110 ,112 ,113 ,114 ,116 ,117 ,118 ,119 ,120 ,121 ,122 ,123 ,123 ,124 ,125 ,125 ,126 ,126 ,126 ,126 ,126 ,
127 ,126 ,126 ,126 ,126 ,126 ,125 ,125 ,124 ,123 ,123 ,122 ,121 ,120 ,119 ,118 ,117 ,116 ,114 ,113 ,112 ,110 ,108 ,107 ,105 ,103 ,102 ,100 ,98 ,96 ,94 ,91 ,
89 ,87 ,85 ,82 ,80 ,78 ,75 ,73 ,70 ,67 ,65 ,62 ,59 ,57 ,54 ,51 ,48 ,45 ,42 ,39 ,36 ,33 ,30 ,27 ,24 ,21 ,18 ,15 ,12 ,9 ,6 ,3 ,
0 ,-3 ,-6 ,-9 ,-12 ,-15 ,-18 ,-21 ,-24 ,-27 ,-30 ,-33 ,-36 ,-39 ,-42 ,-45 ,-48 ,-51 ,-54 ,-57 ,-59 ,-62 ,-65 ,-67 ,-70 ,-73 ,-75 ,-78 ,-80 ,-82 ,-85 ,-87 ,
-89 ,-91 ,-94 ,-96 ,-98 ,-100 ,-102 ,-103 ,-105 ,-107 ,-108 ,-110 ,-112 ,-113 ,-114 ,-116 ,-117 ,-118 ,-119 ,-120 ,-121 ,-122 ,-123 ,-123 ,-124 ,-125 ,-125 ,-126 ,-126 ,-126 ,-126 ,-126 ,
-127 ,-126 ,-126 ,-126 ,-126 ,-126 ,-125 ,-125 ,-124 ,-123 ,-123 ,-122 ,-121 ,-120 ,-119 ,-118 ,-117 ,-116 ,-114 ,-113 ,-112 ,-110 ,-108 ,-107 ,-105 ,-103 ,-102 ,-100 ,-98 ,-96 ,-94 ,-91 ,
-89 ,-87 ,-85 ,-82 ,-80 ,-78 ,-75 ,-73 ,-70 ,-67 ,-65 ,-62 ,-59 ,-57 ,-54 ,-51 ,-48 ,-45 ,-42 ,-39 ,-36 ,-33 ,-30 ,-27 ,-24 ,-21 ,-18 ,-15 ,-12 ,-9 ,-6 ,-3
};

volatile uint16_t Freq_coefficient=1;
volatile int16_t  Amplitude=256;

void setFrequency(uint16_t frequency_Hz)
{
  uint16_t temp= (uint32_t) F_CPU/256*frequency_Hz/256/256;
  cli();
  Freq_coefficient = temp;
  sei();
}

SIGNAL(TIMER1_COMPA_vect)
{
  static uint16_t phase=0;
  volatile int16_t temp;
  
  phase += Freq_coefficient;

  temp  = pgm_read_byte ( &sintab[ phase>>8 ] ) ;

  temp*=Amplitude;
  temp=(temp>>8)+128;
  OCR1A=temp;
  
}

uint16_t LoopTime=10;
float modulationFrequency_Hz=0.1;
    
void loop() 
{ 
    static uint16_t startTime=0;

    float signalFrequency_Hz=( sin( 2 * PI * millis() * 0.001 * modulationFrequency_Hz ) + 1 ) * 1500 + 300;
    setFrequency(signalFrequency_Hz);

    while( (uint16_t) millis() - startTime < LoopTime );
    startTime=millis(); 
}





