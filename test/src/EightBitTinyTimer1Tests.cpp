extern "C" {
    #include "8bit_binary.h"
    #include "8bit_tiny_timer1.h"
    
    #include "TimerSpy.h"
    
    void ISR_TIMER1_COMPA_vect(void);
    void ISR_TIMER1_COMPB_vect(void);
}

#include <stdint.h>
#include "CppUTest/TestHarness.h"

// "virtual" registers to be passed to the timer
static uint8_t virtualGTCCR;
static uint8_t virtualTCCR1;
static uint8_t virtualOCR1A;
static uint8_t virtualOCR1B;
static uint8_t virtualTIMSK;
static uint8_t virtualTCNT1;

static const Timer1Registers timer1Regs = {
    &virtualGTCCR,
    &virtualTCCR1,
    &virtualOCR1A,
    &virtualOCR1B,
    &virtualTIMSK,
    &virtualTCNT1,
};

TEST_GROUP(EightBitTinyTimer1) {
    void setup() {
        virtualGTCCR = 0;
        virtualTCCR1 = 0;
        virtualOCR1A = 0;
        virtualOCR1B = 0;
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
    BYTES_EQUAL(B00000010, virtualGTCCR); // PSM1, reset prescaler
}

TEST(EightBitTinyTimer1, StartTimer) {
    virtualTCCR1 = 0xFF;
    
    timer1_start();
    
    BYTES_EQUAL(B00001011, virtualTCCR1); // prescaler: cpu/1024
}

TEST(EightBitTinyTimer1, StopTimer) {
    virtualGTCCR = 0;
    
    timer1_stop();
    
    BYTES_EQUAL(B00000010, virtualGTCCR);
}

/*
 * Confirm:
 *    TIMSK is set for interrupt on match
 *    OCR1A is set correctly
 */
TEST(EightBitTinyTimer1, AttachOCR1AInterrupt) {
    virtualTCNT1 = 3;
    timer1_attach_interrupt_ocra(42, &timer_spy_handle_interrupt);
    
    BYTES_EQUAL(45, virtualOCR1A);
    BYTES_EQUAL(B01000000, virtualTIMSK); // OCIE1A
}

/*
 * Confirm:
 *    TIMSK is set for interrupt on match
 *    OCR1B is set correctly
 */
TEST(EightBitTinyTimer1, AttachOCR1BInterrupt) {
    virtualTCNT1 = 3;
    timer1_attach_interrupt_ocrb(42, &timer_spy_handle_interrupt);
    
    BYTES_EQUAL(45, virtualOCR1B);
    BYTES_EQUAL(B00100000, virtualTIMSK); // OCIE1B
}

TEST(EightBitTinyTimer1, InvokeCompareA) {
    timer1_attach_interrupt_ocra(42, &timer_spy_handle_interrupt);
    timer1_start();
    
    ISR_TIMER1_COMPA_vect();
    
    BYTES_EQUAL(1, timer_spy_get_interrupt_count());
}

TEST(EightBitTinyTimer1, InvokeCompareB) {
    timer1_attach_interrupt_ocrb(42, &timer_spy_handle_interrupt);
    timer1_start();
    
    ISR_TIMER1_COMPB_vect();
    
    BYTES_EQUAL(1, timer_spy_get_interrupt_count());
}

TEST(EightBitTinyTimer1, PerformReset) {
    virtualTCNT1 = 42;
    
    timer1_set_counter(0);
    
    BYTES_EQUAL(0, virtualTCNT1);
}

TEST(EightBitTinyTimer1, SetRelativeOCRACompare) {
    virtualTCNT1 = 10;
    virtualOCR1A = 15;
    
    timer1_incr_ocra(200);
    BYTES_EQUAL(virtualOCR1A, 210);
}

TEST(EightBitTinyTimer1, SetRelativeOCRBCompare) {
    virtualTCNT1 = 10;
    virtualOCR1B = 15;
    
    timer1_incr_ocrb(200);
    BYTES_EQUAL(virtualOCR1B, 210);
}
