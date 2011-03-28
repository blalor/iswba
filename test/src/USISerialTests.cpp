/*
well, this ought to be interesting, since this is normally completely
in hardware…

USI/UART statically configured for 9600-8-E-1
*/

extern "C" {
    #include "8bit_binary.h"
    #include "usi_serial.h"
    #include "8bit_tiny_timer0.h"

    #include "ByteReceiverSpy.h"
    
    void ISR_PCINT0_vect(void);
    void ISR_TIMER0_COMPA_vect(void);
    void ISR_USI_OVF_vect(void);
}

#include <stdint.h>
#include "CppUTest/TestHarness.h"

static const float _BIT_PERIOD = 1e6/9600.0;

// "virtual" registers to be passed to the USI device driver
static uint8_t virtualPORTB;
static uint8_t virtualPINB;
static uint8_t virtualDDRB;
static uint8_t virtualUSIBR;
static uint8_t virtualUSICR;
static uint8_t virtualUSISR;
static uint8_t virtualGIFR;
static uint8_t virtualGIMSK;
static uint8_t virtualPCMSK;

static const USISerialRxRegisters usiRegs = {
    &virtualPORTB,
    &virtualPINB,
    &virtualDDRB,
    &virtualUSIBR,
    &virtualUSICR,
    &virtualUSISR,
    &virtualGIFR,
    &virtualGIMSK,
    &virtualPCMSK,
};

// virtual registers for timer0
static uint8_t virtualGTCCR;
static uint8_t virtualTCCR0A;
static uint8_t virtualTCCR0B;
static uint8_t virtualOCR0A;
static uint8_t virtualTIMSK;
static uint8_t virtualTCNT0;

static const Timer0Registers timer0Regs = {
    &virtualGTCCR,
    &virtualTCCR0A,
    &virtualTCCR0B,
    &virtualOCR0A,
    &virtualTIMSK,
    &virtualTCNT0,
};

TEST_GROUP(USISerialTests) {
    void setup() {
        virtualPORTB = 0;
        virtualDDRB = 0xff;
        virtualUSIBR = 0;
        virtualUSICR = 0xff;
        virtualUSISR = 0xff;
        virtualGIFR = 0;
        virtualGIMSK = 0;
        virtualPCMSK = 0;

        virtualGTCCR = 0;
        virtualTCCR0A = 0;
        virtualTCCR0B = 0;
        virtualOCR0A = 0;
        virtualTIMSK = 0;
        virtualTCNT0 = 0;
        
        // init byte receiver spy
        brs_init();
        
        // must initialize Timer0 first
        timer0_init(&timer0Regs, TIMER0_PRESCALE_8);
        usi_serial_receiver_init(&usiRegs, &brs_receive_byte);
    }
};

TEST(USISerialTests, ClockSpeed) {
    // sanity check; need to change if CPU clock changes, but the timer 
    // config is hard-coded.
    CHECK(F_CPU == 8000000);
}

TEST(USISerialTests, Initialization) {
    virtualPORTB = 0x0;
    virtualDDRB = 0xff;
    virtualUSICR = 0xff;
    virtualGIMSK = 0;
    virtualPCMSK = 0;
    
    usi_serial_receiver_init(&usiRegs, &brs_receive_byte);

    BYTES_EQUAL(B00000001, virtualPORTB); // DI pull-up enabled
    BYTES_EQUAL(B11111110, virtualDDRB);  // DI configured for input
    BYTES_EQUAL(B00000000, virtualUSICR); // USI disabled
    BYTES_EQUAL(B00100000, virtualGIMSK); // PCINTs enabled
    BYTES_EQUAL(B00000001, virtualPCMSK); // PCINT0 enabled
    
    BYTES_EQUAL(B10000001, virtualGTCCR);  // timer stopped
    BYTES_EQUAL(0,         virtualTCCR0B); // timer stopped, no prescaler
    BYTES_EQUAL(B00010000, virtualTIMSK);  // OCR0A comp match interrupt enabled
}

TEST(USISerialTests, HandleStartBit) {
    /*
    the DI line idles high; need to trigger the pin-change interrupt, 
    emulating a change from high to low.
    */
    
    virtualPINB  = B11111110;
    virtualTCNT0 = 0;
    virtualGTCCR = 0xff;
    virtualPCMSK = 0xff;
    
    // fire the PCI
    ISR_PCINT0_vect();
    
    // ----- check Timer0 started
    BYTES_EQUAL(B00000010, virtualTCCR0B); // prescaler; cpu/8
    BYTES_EQUAL(B01111111, virtualGTCCR);  // TSM bit cleared
    
    // check Timer0 configured to compare with OCR0A at 1.5 times the bit 
    // period, plus the interrupt latency
    DOUBLES_EQUAL(
        (_BIT_PERIOD * 1.5) + INTERRUPT_STARTUP_DELAY,
        virtualOCR0A,
        ((_BIT_PERIOD * 1.5) + INTERRUPT_STARTUP_DELAY)*0.02 // 2%
    );
    
    // ----- check USI config
    
    // clear USI status interrupt flags; don't care about bit 4 (USIDC)
    BYTES_EQUAL(0x0f, virtualUSISR >> 4); 
    BYTES_EQUAL(8, 0x0f & virtualUSISR);

    // USI overflow interrupt, 3-wire mode, Timer0 compare as clock source
    BYTES_EQUAL(B01010100, virtualUSICR);
    
    BYTES_EQUAL(B11111110, virtualPCMSK); // PCI disabled
}

TEST(USISerialTests, HandleTimerReload) {
    // fire the PCI
    ISR_PCINT0_vect();
    
    // depends on proper initialization of the USI UART, which passes a
    // reference to its OCR0A handler to timer0. usi_serial and
    // 8bit_tiny_timer0 are already fairly coupled, so I guess this is ok, but
    // it doesn't sit quite right with me…
    
    virtualTCNT0 = 11;
    ISR_TIMER0_COMPA_vect();
    BYTES_EQUAL(TIMER0_SEED + 11, virtualOCR0A);
}

TEST(USISerialTests, HandleByteReceivedPlusParityBit) {
    uint8_t compa_check_val;
    
    ISR_PCINT0_vect();
    
    // fire the compare interrupt as configured to do so
    // not really necessary, just because I can
    for (uint8_t i = (0x0f & virtualUSISR); i < USI_COUNTER_MAX_COUNT; i++) {
        virtualTCNT0 = virtualOCR0A;
        compa_check_val = virtualTCNT0 + TIMER0_SEED;
        
        ISR_TIMER0_COMPA_vect();
        
        BYTES_EQUAL(compa_check_val, virtualOCR0A);
        // @todo anything of value to check here?
    }
    
    // assume USI is configured correctly and has received 8 bits, in reverse
    // order. we're going to pretend an 'a' has been sent
                   
    virtualUSIBR = B10000110; // 'a' reversed
    virtualTCCR0B = 0xff;
    virtualPCMSK = 0;
    ISR_USI_OVF_vect();
    
    BYTES_EQUAL(1, brs_get_invocation_count());
    BYTES_EQUAL('a', brs_get_received_byte());
    
    // ----- check config for/before "consuming" parity bit
    BYTES_EQUAL(0xff, virtualTCCR0B); // no change, yet
    BYTES_EQUAL(0,    virtualPCMSK); // no change, yet
    
    // clear USI status interrupt flags; don't care about bit 4 (USIDC)
    BYTES_EQUAL(0x0f, virtualUSISR >> 4); 
    BYTES_EQUAL(15, 0x0f & virtualUSISR);  // just one bit left to consume

    for (uint8_t i = (0x0f & virtualUSISR); i < USI_COUNTER_MAX_COUNT; i++) {
        virtualTCNT0 = virtualOCR0A;
        compa_check_val = virtualTCNT0 + TIMER0_SEED;
        
        ISR_TIMER0_COMPA_vect();
        
        BYTES_EQUAL(compa_check_val, virtualOCR0A);
        // @todo anything of value to check here?
    }
    
    ISR_USI_OVF_vect();
    
    BYTES_EQUAL(1, brs_get_invocation_count()); // still called just once
    
    BYTES_EQUAL(B11111000, virtualTCCR0B); // timer0 prescaler cleared
    BYTES_EQUAL(0,         virtualUSICR); // USI disabled
    BYTES_EQUAL(B00000001, virtualPCMSK); // PCINT0 re-enabled
    // @todo confirm other register settings
}
