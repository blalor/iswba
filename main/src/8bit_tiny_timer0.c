#include "8bit_tiny_timer0.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static const Timer0Registers *registers;
static Timer0Prescale prescale;

static void (*ocra_handler)(void);

void timer0_init(const Timer0Registers *regs, const Timer0Prescale _prescale) {
    registers = regs;
    prescale = _prescale;
    
    // halt all timers
    *registers->pGTCCR |= _BV(TSM) | _BV(PSR0);
}

void timer0_start() {
    // set prescaler
    *registers->pTCCR0B = prescale;
    *registers->pGTCCR &= ~_BV(TSM);
}

void timer0_stop() {
    // clear the prescaler
    *registers->pTCCR0B &= TIMER0_PRESCALE_OFF;
}

void timer0_set_counter(const uint8_t counter_val) {
    *registers->pTCNT0 = counter_val;
}

void timer0_attach_interrupt_ocra(const uint8_t counter_val, void (*handler)(void)) {
    *registers->pTIMSK = _BV(OCIE0A);
    timer0_incr_ocra(counter_val);
    
    ocra_handler = handler;
}

void timer0_incr_ocra(const uint8_t timer_inc) {
    *registers->pOCR0A = *registers->pTCNT0 + timer_inc;
}

ISR(TIMER0_COMPA_vect) {
    ocra_handler();
}
