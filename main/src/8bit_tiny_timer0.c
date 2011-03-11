#include "8bit_tiny_timer0.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static const Timer0Registers *registers;
static void (*ocra_handler)(void);

void timer0_init(const Timer0Registers *regs, const Timer0Prescale prescale) {
    registers = regs;
    
    // halt all timers
    *registers->pGTCCR |= _BV(TSM) | _BV(PSR0);
    
    // configure for CTC (clear-on-timer-compare) and set prescaler
    *registers->pTCCR0A = _BV(WGM01);
    *registers->pTCCR0B = prescale;
}

void timer0_attach_interrupt_ocra(const uint8_t counter_val, void (*handler)(void)) {
    *registers->pTIMSK = _BV(OCIE0A);
    *registers->pOCR0A = counter_val;
    
    ocra_handler = handler;
}

void timer0_reset() {
    *registers->pTCNT0 = 0;
}

void timer0_start() {
    *registers->pGTCCR &= ~_BV(TSM);
}

ISR(TIMER0_COMPA_vect) {
    ocra_handler();
}
