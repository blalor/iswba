#ifndef MOCK_AVR_IO_H
#define MOCK_AVR_IO_H

#include <avr/sfr_defs.h>

// GTCCR
#define PSR0 0
#define PSR1 1





#define TSM  7

// TCCR0A
#define WGM01 1

// TCCR0B
#define CS00 0
#define CS01 1
#define CS02 2

// TCCR1
#define CS13 3

#define CS11 1
#define CS10 0

// TIMSK
#define OCIE0A 4
#define OCIE1B 5
#define OCIE1A 6

// PORTB
#define PB0 0

// GIFR
#define PCIF 5

// GIMSK
#define PCIE 5

// PCMSK
#define PCINT0 0

// USICR
#define USISIE 7
#define USIOIE 6
#define USIWM1 5
#define USIWM0 4
#define USICS1 3
#define USICS0 2
#define USICLK 1
#define USITC  0


#endif
