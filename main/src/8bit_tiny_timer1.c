#include "8bit_tiny_timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static const Timer1Registers *registers;
static void (*ocra_handler)(void);

void timer1_init(const Timer1Registers *regs, const Timer1Prescale prescale) {
    registers = regs;
    
    // halt all timers
    *registers->pGTCCR |= _BV(PSR1);
    
    // set prescaler
    *registers->pTCCR1 = prescale;
}

void timer1_attach_interrupt_ocra(const uint8_t counter_val, void (*handler)(void)) {
    *registers->pTIMSK = _BV(OCIE1A);
    *registers->pOCR1A = counter_val;
    
    ocra_handler = handler;
}

void timer1_reset() {
    *registers->pTCNT1 = 0;
}

ISR(TIMER1_COMPA_vect) {
    ocra_handler();
}
