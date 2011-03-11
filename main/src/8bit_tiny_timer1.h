#ifndef EIGHT_BIT_TINY_TIMER1_H
#define EIGHT_BIT_TINY_TIMER1_H

#include <avr/io.h>
#include <stdint.h>

typedef struct __timer1_regs {
    volatile uint8_t *pGTCCR;
    volatile uint8_t *pTCCR1;
    volatile uint8_t *pOCR1A;
    volatile uint8_t *pTIMSK;
    volatile uint8_t *pTCNT1;
} Timer1Registers;

typedef enum __prescalers1 {
    TIMER1_PRESCALE_1024 = _BV(CS13)             | _BV(CS11) | _BV(CS10),
} Timer1Prescale;

void timer1_init(const Timer1Registers *regs, const Timer1Prescale prescale);

void timer1_attach_interrupt_ocra(const uint8_t counter_val, void (*handler)(void));
void timer1_reset(void);

void timer1_start(void);

#endif