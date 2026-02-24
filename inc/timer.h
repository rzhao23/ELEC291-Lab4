#ifndef TIMER_H
#define TIMER_H

#include <EFM8LB1.h>

#define SYSCLK      72000000L  // SYSCLK frequency in Hz
#define BAUDRATE      115200L  // Baud rate of UART in bps

extern unsigned char overflow_count;

char _c51_external_startup (void);
void Timer3us(unsigned char us);
void waitms (unsigned int ms);
void TIMER0_Init(void);

#endif
