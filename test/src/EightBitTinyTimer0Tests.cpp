extern "C" {
    #include "8bit_binary.h"
    #include "8bit_tiny_timer0.h"
    
    #include "TimerSpy.h"
    
    void ISR_TIMER0_COMPA_vect(void);
}

#include <stdint.h>
#include "CppUTest/TestHarness.h"

// "virtual" registers to be passed to the timer
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

TEST(EightBitTinyTimer0, Initialization) {
    // setup calls init
    BYTES_EQUAL(B10000001, virtualGTCCR);  // TSM, PSM0; reset prescaler, stop timer
}

TEST(EightBitTinyTimer0, StartTimer) {
    virtualGTCCR = 0xFF;
    
    timer0_start();
    
    BYTES_EQUAL(B01111111, virtualGTCCR);  // TSM cleared
    BYTES_EQUAL(B00000101, virtualTCCR0B); // prescaler: cpu/1024
}

TEST(EightBitTinyTimer0, StopTimer) {
    virtualGTCCR = 0;
    
    timer0_stop();
    
    BYTES_EQUAL(B00000001, virtualGTCCR);
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

TEST(EightBitTinyTimer0, CallInterrupt) {
    timer0_attach_interrupt_ocra(42, &timer_spy_handle_interrupt);
    timer0_start();
    
    ISR_TIMER0_COMPA_vect();
    
    BYTES_EQUAL(1, timer_spy_get_interrupt_count());
}

TEST(EightBitTinyTimer0, PerformReset) {
    virtualTCNT0 = 42;
    
    timer0_set_counter(0);
    
    BYTES_EQUAL(0, virtualTCNT0);
}

TEST(EightBitTinyTimer0, SetRelativeOCRACompare) {
    virtualTCNT0 = 10;
    virtualOCR0A = 15;
    
    timer0_incr_ocra(200);
    BYTES_EQUAL(virtualOCR0A, 210);
}
