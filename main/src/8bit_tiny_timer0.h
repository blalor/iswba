#ifndef EIGHT_BIT_TINY_TIMER0_H
#define EIGHT_BIT_TINY_TIMER0_H

#include <avr/io.h>
#include <stdint.h>

typedef struct __timer0_regs {
    volatile uint8_t *pGTCCR;
    volatile uint8_t *pTCCR0A;
    volatile uint8_t *pTCCR0B;
    volatile uint8_t *pOCR0A;
    volatile uint8_t *pTIMSK;
    volatile uint8_t *pTCNT0;
} Timer0Registers;

typedef enum __prescalers {
    // TIMER0_PRESCALE_1 =                            _BV(CS00),
    TIMER0_PRESCALE_8 =                _BV(CS01)            ,
    // TIMER0_PRESCALE_64 =               _BV(CS01) | _BV(CS00),
    // TIMER0_PRESCALE_256 =  _BV(CS02)                        ,
    TIMER0_PRESCALE_1024 = _BV(CS02) |             _BV(CS00),
} Timer0Prescale;

// configure registers, prescaler. does not start timer.
void timer0_init(const Timer0Registers *regs, const Timer0Prescale prescale);

// starts timer with prescaler given in _init
void timer0_start(void);

// stops timer (clears prescaler)
void timer0_stop(void);

// sets OCR0A to TCNT0 plus timer_inc
void timer0_incr_ocra(const uint8_t compare_inc);

// attaches an interrupt handler for TIMER0_COMPA and sets initial value for
// OCR0A
void timer0_attach_interrupt_ocra(const uint8_t compare_val, void (*handler)(void));

// sets the counter to the given value
void timer0_set_counter(const uint8_t counter_val);

#endif