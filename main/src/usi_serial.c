#include "usi_serial.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "8bit_tiny_timer0.h"

static const USISerialRxRegisters *reg;
static void (*received_byte_handler)(uint8_t);

// not part of the public interface
static void usi_handle_ocra_reload(void);

// Reverses the order of bits in a byte.
// I.e. MSB is swapped with LSB, etc.
static inline uint8_t reverse_bits(const uint8_t to_swap) {
    uint8_t x = to_swap;
    
    x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
    x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
    x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
    return x;    
}

void usi_serial_receiver_init(const USISerialRxRegisters *_reg,
                              void (*_handler)(uint8_t))
{
    reg = _reg;
    received_byte_handler = _handler;
    
    *reg->pDDRB  &= ~_BV(PB0);   // set RX pin as input
    *reg->pPORTB |= _BV(PB0);    // enable pull-up on RX pin
    *reg->pUSICR  = 0;           // disable USI
    *reg->pGIFR  &= ~_BV(PCIF);  // clear PCI flag, just because
    *reg->pGIMSK |= _BV(PCIE);   // enable PCIs
    *reg->pPCMSK |= _BV(PCINT0); // enable PCINT0
    
    // initial OCR0A value isn't important
    timer0_attach_interrupt_ocra(0, &usi_handle_ocra_reload);
}

// @todo refactor this so that the PCINT0 ISR is configured in main()
ISR(PCINT0_vect) {
    if ((*reg->pPINB & ~_BV(PB0)) == 0) {
        // PB0 is low; start bit received
        // do the time-critical stuff first
        
        // @todo once I've figured out which compare register is used to clock
        // the USI, it may be possible and better to use CTC mode. Should be a
        // little less code and more stable timing
        
        // start the timer and configure it to fire the OCR0A compare interrupt
        // in the middle of the first data bit
        timer0_set_counter(0);
        timer0_start();
        timer0_incr_ocra(INITIAL_TIMER0_SEED + INTERRUPT_STARTUP_DELAY);
        
        // ----- configure the USI
        // clear interrupt flags, prepare for data bit count
        // overflow should occur when all data bits are received
        *reg->pUSISR = 0xF0 | USI_COUNTER_RECEIVE_SEED; 
        
        // enable overflow interrupt, set 3-wire mode, clock from timer0 comp
        *reg->pUSICR = _BV(USIOIE) | _BV(USIWM0) | _BV(USICS0);
        
        // ----- time-critical stuff done
        
        *reg->pPCMSK &= ~_BV(PCINT0); // disable PCINT0
    }
}

static void usi_handle_ocra_reload() {
    timer0_incr_ocra(TIMER0_SEED);
}

// USI overflow interrupt.  Configured to occur when the desired number of bits
// have been shifted in (in reverse order!)
ISR(USI_OVF_vect) {
    // WARNING! this is being called in an ISR and MUST be very fast!
    received_byte_handler(reverse_bits(*reg->pUSIBR));
    
    // disable timer
    timer0_stop();
    
    *reg->pUSICR = 0;            // disable USI
    *reg->pPCMSK |= _BV(PCINT0); // re-enable PCINT
}
