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
    timer0_stop();
}

void timer0_start() {
    *registers->pGTCCR |= _BV(TSM) | _BV(PSR0);
    
    // @todo cleanup
    // set prescaler
    *registers->pTCCR0B &= TIMER0_PRESCALE_OFF; // clear just prescaler bits
    *registers->pTCCR0B |= prescale; // set just prescaler bits
    
    *registers->pTIFR   |= _BV(OCF0A); // clear output compare A flag
    
    *registers->pGTCCR &= ~_BV(TSM);
}

void timer0_stop() {
    // clear the prescaler
    *registers->pTCCR0B &= TIMER0_PRESCALE_OFF;
}

void timer0_set_counter(const uint8_t counter_val) {
    *registers->pTCNT0 = counter_val;
}

void timer0_set_ocra_interrupt_handler(void (*handler)(void)) {
    ocra_handler = handler;
}

void timer0_set_ocra(const uint8_t val) {
    *registers->pOCR0A = val;
}

void timer0_incr_ocra(const uint8_t timer_inc) {
    timer0_set_ocra(*registers->pTCNT0 + timer_inc);
}

void timer0_enable_ctc() {
    // WGM0[2:0] = 010
    *registers->pTCCR0B &= ~_BV(WGM02);
    *registers->pTCCR0A |=  _BV(WGM01);
    *registers->pTCCR0A &= ~_BV(WGM00);
}

void timer0_enable_ocra_interrupt() {
    *registers->pTIMSK |= _BV(OCIE0A);
}

void timer0_disable_ocra_interrupt() {
    *registers->pTIMSK &= ~_BV(OCIE0A);
}

ISR(TIMER0_COMPA_vect) {
    ocra_handler();
}
