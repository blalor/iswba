/*
    Limited implementation of Timer1, based on the ATtiny85 datasheet.  
    Supports invoking a method via an ISR when OCR1A matches.
*/
#include "8bit_tiny_timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static const Timer1Registers *registers;

static uint8_t ocra_incr;
static void (*ocra_handler)(void);

static uint8_t ocrb_incr;
static void (*ocrb_handler)(void);

void timer1_init(const Timer1Registers *regs, const Timer1Prescale prescale) {
    registers = regs;
    
    // resets the prescaler (is this really necessary?)
    *registers->pGTCCR |= _BV(PSR1);
    
    // set prescaler
    *registers->pTCCR1 = prescale;
}

void timer1_attach_interrupt_ocra(void (*handler)(void), const uint8_t incr_val) {
    ocra_incr = incr_val;
    ocra_handler = handler;
    
    *registers->pTIMSK = _BV(OCIE1A);
    timer1_reset_ocra();
}

void timer1_reset_ocra() {
    *registers->pOCR1A = *registers->pTCNT1 + ocra_incr;
}

void timer1_attach_interrupt_ocrb(void (*handler)(void), const uint8_t incr_val) {
    ocrb_incr = incr_val;
    ocrb_handler = handler;
    
    *registers->pTIMSK = _BV(OCIE1B);
    timer1_reset_ocrb();
}

void timer1_reset_ocrb() {
    *registers->pOCR1B = *registers->pTCNT1 + ocrb_incr;
}

ISR(TIMER1_COMPA_vect) {
    ocra_handler();
}

ISR(TIMER1_COMPB_vect) {
    ocrb_handler();
}
