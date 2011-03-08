#include "8bit_tiny_timer0.h"

#include <avr/interrupt.h>

static const Timer0Registers *registers;
static void (*ocra_handler)(void);

void timer0_init(const Timer0Registers *regs, const Timer0Prescale prescale) {
    registers = regs;
    
    // halt all timers
    *registers->pGTCCR |= (1 << 7) | (1 << 0);
    
    // configure for CTC (clear-on-timer-compare) and set prescaler
    *registers->pTCCRxA = (1 << 1);
    *registers->pTCCRxB = prescale;
}

void timer0_attach_interrupt_ocra(const uint8_t counter_val, void (*handler)(void)) {
    *registers->pTIMSK = (1 << 4);
    *registers->pOCRxA = counter_val;
    
    ocra_handler = handler;
}

void timer0_reset() {
    *registers->pTCNTx = 0;
}

void timer0_start() {
    *registers->pGTCCR &= ~(1 << 7);
}

ISR(TIMER0_COMPA_vect) {
    ocra_handler();
}
