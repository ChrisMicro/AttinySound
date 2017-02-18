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
  v0.2  18.2.2017 C. -H-A-B-E-R-E-R-  improved version, intterupt protected setup functions  

  It is mandatory to keep the list of authors in this code.
  Please add your name if you improve/extend something


*/

void setup () 
{
  
  cli();

  // Set up Timer/Counter1 31250Hz
  TCCR1 &= ~(7<<CS10);   // make sure bits are empty
  TCCR1 |= 1<<CS11;      // set prescaler

  // pulse width modulator A enable
  TCCR1 &= ~(1 << COM1A0);       // make sure register is empty
  TCCR1 |=   1 << COM1A1 ;       // PWM mode: clear the OC1A output line on compare match

  TCCR1 |= 1 << PWM1A;

  TIMSK |= 1 << OCIE1A;          // Enable compare match interrupt for timer 1

  OCR1A = 128;                   // initialize PWM to 50% mean value

  DDRB|= 1<<PB1; // PWM pin as output

  sei();
  setFrequency(880);
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

int8_t sin_int8(uint8_t index)
{
  return pgm_read_byte( &sintab[index] );
}

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
  static uint16_t phase=0;
  int16_t temp;

  
  phase += Freq_coefficient;

  temp  = (int8_t) pgm_read_byte ( &sintab[ phase>>8 ] ) ;

  temp *= Amplitude;
  temp = (temp>>8)+128;
  OCR1A=temp;
    
}

// analog read scaled for the 8BitMixTape voltage divider
uint16_t analogReadScaled(uint8_t channel)
{
  uint16_t value=analogRead(channel);
  if(value>511) value=511;
  return value*2;
}

uint16_t LoopTime_ms = 10;
float AmplitudeModulationFrequency_Hz = 3;
float FrequencyModulationFrequency_Hz = 0.1;

#define POTI_LEFT      A1
#define POTI_RIGHT     A2

void loop()
{
  static uint16_t startTime_ms = 0;

  static uint16_t phase_sig1=0;
  static uint16_t phase_sig2=0;

  // wait for loop cycle
  while ( (uint16_t) millis() - startTime_ms < LoopTime_ms );
  startTime_ms = millis();
      
  AmplitudeModulationFrequency_Hz = analogReadScaled(POTI_LEFT) * 0.05;
  phase_sig1 += LoopTime_ms * 0.001 * AmplitudeModulationFrequency_Hz * 256 *256;
  setAmplitude( sin_int8(phase_sig1>>8)+128 );
  
  
  FrequencyModulationFrequency_Hz = analogReadScaled(POTI_RIGHT) * 0.1;
  phase_sig2 += LoopTime_ms * 0.001 * FrequencyModulationFrequency_Hz * 256 *256;
  float signalFrequency_Hz =(float) (sin_int8(phase_sig2>>8)+128)/255 * 1500 + 300;
  setFrequency(signalFrequency_Hz);
  
}

