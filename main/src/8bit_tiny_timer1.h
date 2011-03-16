#ifndef EIGHT_BIT_TINY_TIMER1_H
#define EIGHT_BIT_TINY_TIMER1_H

#include <avr/io.h>
#include <stdint.h>

typedef struct __timer1_regs {
    volatile uint8_t *pGTCCR;
    volatile uint8_t *pTCCR1;
    volatile uint8_t *pOCR1A;
    volatile uint8_t *pOCR1B;
    volatile uint8_t *pTIMSK;
    volatile uint8_t *pTCNT1;
} Timer1Registers;

typedef enum __prescalers1 {
    TIMER1_PRESCALE_128  = _BV(CS13)                                    ,
    TIMER1_PRESCALE_1024 = _BV(CS13)             | _BV(CS11) | _BV(CS10),
} Timer1Prescale;

// configure registers, prescaler. does not start timer.
void timer1_init(const Timer1Registers *regs, const Timer1Prescale prescale);

// starts timer with prescaler given in _init
void timer1_start(void);

// stops timer (clears prescaler)
void timer1_stop(void);

// sets the counter to the given value
void timer1_set_counter(const uint8_t counter_val);

// sets OCR1A to TCNT1 plus timer_inc
void timer1_incr_ocra(const uint8_t compare_inc);

// attaches an interrupt handler for TIMER1_COMPA and sets initial value for
// OCR1A
void timer1_attach_interrupt_ocra(const uint8_t compare_val, void (*handler)(void));

// sets OCR1B to TCNT1 plus timer_inc
void timer1_incr_ocrb(const uint8_t compare_inc);

// attaches an interrupt handler for TIMER1_COMPB and sets initial value for
// OCR1B
void timer1_attach_interrupt_ocrb(const uint8_t compare_val, void (*handler)(void));

#endif
