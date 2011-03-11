extern "C" {
    #include "8bit_binary.h"
    #include "8bit_tiny_timer1.h"
    
    #include "TimerSpy.h"
    
    void ISR_TIMER1_COMPA_vect(void);
}

#include <stdint.h>
#include "CppUTest/TestHarness.h"

// "virtual" registers to be passed to the timer
static uint8_t virtualGTCCR;
static uint8_t virtualTCCR1;
static uint8_t virtualOCR1A;
static uint8_t virtualTIMSK;
static uint8_t virtualTCNT1;

static const Timer1Registers timer1Regs = {
    &virtualGTCCR,
    &virtualTCCR1,
    &virtualOCR1A,
    &virtualTIMSK,
    &virtualTCNT1,
};

TEST_GROUP(EightBitTinyTimer1) {
    void setup() {
        virtualGTCCR = 0;
        virtualTCCR1 = 0;
        virtualOCR1A = 0;
        virtualTIMSK = 0;
        virtualTCNT1 = 0;

        timer1_init(&timer1Regs, TIMER1_PRESCALE_1024);
        
        timer_spy_reset();
    }
};

/*
 * Confirm:
 *    GTCCR is set to shut down timer during initialization.
 *    TCCR1 is set to correct prescaler (cpu/1024)
 */
TEST(EightBitTinyTimer1, Initialization) {
    // setup calls init
    BYTES_EQUAL(B00000010, virtualGTCCR);
    BYTES_EQUAL(B00001011, virtualTCCR1); // prescaler: cpu/1024
}

/*
 * Confirm:
 *    TIMSK  is set for interrupt on match
 */
TEST(EightBitTinyTimer1, AttachOCR1AInterrupt) {
    timer1_attach_interrupt_ocra(42, &timer_spy_handle_interrupt);
    
    BYTES_EQUAL(42, virtualOCR1A);
    BYTES_EQUAL(B01000000, virtualTIMSK); // OCIE1A
}

TEST(EightBitTinyTimer1, CallInterrupt) {
    timer1_attach_interrupt_ocra(42, &timer_spy_handle_interrupt);
    
    ISR_TIMER1_COMPA_vect();
    
    BYTES_EQUAL(1, timer_spy_get_interrupt_count());
}

TEST(EightBitTinyTimer1, PerformReset) {
    virtualTCNT1 = 42;
    
    timer1_reset();
    
    BYTES_EQUAL(0, virtualTCNT1);
}
