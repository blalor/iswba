/*
    Limited implementation of Timer1, based on the ATtiny85 datasheet.  
    Supports invoking a method via an ISR when OCR1A matches.
*/
#include "8bit_tiny_timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static const Timer1Registers *registers;
static Timer1Prescale prescale;

static void (*ocra_handler)(void);
static void (*ocrb_handler)(void);

void timer1_init(const Timer1Registers *regs, const Timer1Prescale _prescale) {
    registers = regs;
    prescale = _prescale;
    
    // resets the prescaler (is this really necessary?)
    *registers->pGTCCR |= _BV(PSR1);
}

void timer1_start() {
    // set prescaler
    *registers->pTCCR1 = prescale;
}

void timer1_stop() {
    // reset the prescaler, which should stop the timer
    *registers->pGTCCR |= _BV(PSR1);
}

void timer1_set_counter(const uint8_t counter_val) {
    *registers->pTCNT1 = counter_val;
}

void timer1_attach_interrupt_ocra(const uint8_t counter_val, void (*handler)(void)) {
    *registers->pTIMSK = _BV(OCIE1A);
    timer1_incr_ocra(counter_val);
    
    ocra_handler = handler;
}

void timer1_incr_ocra(const uint8_t timer_inc) {
    *registers->pOCR1A = *registers->pTCNT1 + timer_inc;
}

void timer1_attach_interrupt_ocrb(const uint8_t counter_val, void (*handler)(void)) {
    *registers->pTIMSK = _BV(OCIE1B);
    timer1_incr_ocrb(counter_val);
    
    ocrb_handler = handler;
}

void timer1_incr_ocrb(const uint8_t timer_inc) {
    *registers->pOCR1B = *registers->pTCNT1 + timer_inc;
}

ISR(TIMER1_COMPA_vect) {
    ocra_handler();
}

ISR(TIMER1_COMPB_vect) {
    ocrb_handler();
}
