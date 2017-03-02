#include "stdint.h"
#include <avr/io.h>

#define TIMER TCNT1 // we use timer1 for measuring time
#define FRAMESIZE       2

extern uint8_t FrameData[ FRAMESIZE ];
uint8_t receiveFrame();
void  frameRecieverBegin();
