extern "C" {
    #include "8bit_binary.h"
    #include "8bit_tiny_timer0.h"
    
    #include "TimerSpy.h"
    
    void ISR_TIMER0_COMPA_vect(void);
}

#include <stdint.h>
#include "CppUTest/TestHarness.h"

uint8_t virtualGTCCR;
uint8_t virtualTCCR0A;
uint8_t virtualTCCR0B;
uint8_t virtualOCR0A;
uint8_t virtualTIMSK;
uint8_t virtualTCNT0;

const Timer0Registers timer0Regs = {
    &virtualGTCCR,
    &virtualTCCR0A,
    &virtualTCCR0B,
    &virtualOCR0A,
    &virtualTIMSK,
    &virtualTCNT0
};


TEST_GROUP(EightBitTinyTimer0) {
    void setup() {
        virtualGTCCR = 0;
        virtualTCCR0A = 0;
        virtualTCCR0B = 0;
        virtualOCR0A = 0;
        virtualTIMSK = 0;
        virtualTCNT0 = 0;

        timer0_init(&timer0Regs, TIMER0_PRESCALE_1024);
        
        timer_spy_reset();
    }
};

/*
 * Confirm:
 *    GTCCR  is set to shut down timer during initialization.
 *    TCCR0A, TCCR0B is set to clear on timer compare, correct prescaler (cpu/1024)
 */
TEST(EightBitTinyTimer0, Initialization) {
    // setup calls init
    BYTES_EQUAL(B10000001, virtualGTCCR);
    BYTES_EQUAL(B00000010, virtualTCCR0A); // CTC
    BYTES_EQUAL(B00000101, virtualTCCR0B); // prescaler: cpu/1024
}

/*
 * Confirm:
 *    TIMSK  is set for interrupt on match
 */
TEST(EightBitTinyTimer0, AttachOCRxAInterrupt) {
    timer0_attach_interrupt_ocra(42, &timer_spy_handle_interrupt);
    
    BYTES_EQUAL(42, virtualOCR0A);
    BYTES_EQUAL(B00010000, virtualTIMSK); // OCIE0A
}

/*
 * Confirm:
 *    GTCCR is set to allow timers to run
 */
TEST(EightBitTinyTimer0, Start) {
    timer0_start();
    
    BYTES_EQUAL(B00000001, virtualGTCCR);
}

TEST(EightBitTinyTimer0, CallInterrupt) {
    timer0_attach_interrupt_ocra(42, &timer_spy_handle_interrupt);
    timer0_start();
    
    ISR_TIMER0_COMPA_vect();
    
    BYTES_EQUAL(1, timer_spy_get_interrupt_count());
}

TEST(EightBitTinyTimer0, PerformReset) {
    virtualTCNT0 = 42;
    
    timer0_reset();
    
    BYTES_EQUAL(0, virtualTCNT0);
}
