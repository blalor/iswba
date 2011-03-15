#include "usi_serial.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "8bit_tiny_timer0.h"

static const USISerialRxRegisters *reg;

void usi_serial_receiver_init(const USISerialRxRegisters *_reg) {
    reg = _reg;
    
    *reg->pDDRB  &= ~_BV(PB0);   // set RX pin as input
    *reg->pPORTB |= _BV(PB0);    // enable pull-up on RX pin
    *reg->pUSICR  = 0;           // disable USI
    *reg->pGIFR  &= ~_BV(PCIF);  // clear PCI flag, just because
    *reg->pGIMSK |= _BV(PCIE);   // enable PCIs
    *reg->pPCMSK |= _BV(PCINT0); // enable PCINT0
    
    // initial OCR0A value isn't important
    timer0_attach_interrupt_ocra(0, NULL);
}

ISR(PCINT0_vect) {
    if ((*reg->pPINB & ~_BV(PB0)) == 0) {
        // PB0 is low; start bit received
        // do the time-critical stuff first
        
        // start the timer and configure it to fire the OCR0A compare interrupt
        // in the middle of the first data bit
        timer0_set_counter(0);
        timer0_start();
        timer0_incr_ocra(INITIAL_TIMER0_SEED + INTERRUPT_STARTUP_DELAY);
        
        // ----- configure the USI
        // clear interrupt flags, prepare for data bit count
        *reg->pUSISR = 0xF0 | USI_COUNTER_RECEIVE_SEED; 
        
        // enable overflow interrupt, set 3-wire mode, clock from timer0 comp
        *reg->pUSICR = _BV(USIOIE) | _BV(USIWM0) | _BV(USICS0);
        
        // ----- time-critical stuff done
        
        *reg->pPCMSK &= ~_BV(PCINT0); // disable PCINT0
    }
}
