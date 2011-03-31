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
static uint8_t virtualTIFR;
static uint8_t virtualTCNT0;

static const Timer0Registers timer0Regs = {
    &virtualGTCCR,
    &virtualTCCR0A,
    &virtualTCCR0B,
    &virtualOCR0A,
    &virtualTIMSK,
    &virtualTIFR,
    &virtualTCNT0
};

TEST_GROUP(EightBitTinyTimer0) {
    void setup() {
        virtualGTCCR = 0;
        virtualTCCR0A = 0;
        virtualTCCR0B = 0xff;
        virtualOCR0A = 0;
        virtualTIMSK = 0;
        virtualTIFR = 0;
        virtualTCNT0 = 0;

        timer0_init(&timer0Regs, TIMER0_PRESCALE_1024);
        
        timer_spy_reset();
    }
};

TEST(EightBitTinyTimer0, Initialization) {
    // setup calls init
    BYTES_EQUAL(B11111000, virtualTCCR0B);  // prescaler cleared
}

TEST(EightBitTinyTimer0, StartTimer) {
    virtualGTCCR = 0xFF;
    
    timer0_start();
    
    BYTES_EQUAL(B01111111, virtualGTCCR);  // TSM cleared
    BYTES_EQUAL(B11111101, virtualTCCR0B); // prescaler: cpu/1024
}

TEST(EightBitTinyTimer0, StopTimer) {
    virtualTCCR0B = 0xff;
    
    timer0_stop();
    
    BYTES_EQUAL(B11111000, virtualTCCR0B);
}

/*
 * Confirm:
 *    TIMSK  is set for interrupt on match
 */
TEST(EightBitTinyTimer0, AttachOCRxAInterrupt) {
    virtualOCR0A = 0;
    virtualTIMSK = 0;
    
    timer0_set_ocra_interrupt_handler(&timer_spy_handle_interrupt);
    
    BYTES_EQUAL(0, virtualOCR0A); // no change to OCR0A
    BYTES_EQUAL(0, virtualTIMSK); // no interrupts enabled
}

TEST(EightBitTinyTimer0, CallInterrupt) {
    timer0_set_ocra_interrupt_handler(&timer_spy_handle_interrupt);
    
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
    BYTES_EQUAL(210, virtualOCR0A);
}

TEST(EightBitTinyTimer0, EnableCTCMode) {
    virtualTCCR0A = B00000001;
    virtualTCCR0B = 0xff;
    
    timer0_enable_ctc();
    
    // WGM02:WGM01:WGM00 = 0:1:0
    BYTES_EQUAL(B00000010, virtualTCCR0A); // WGM01 set, WGM00 cleared
    BYTES_EQUAL(B11110111, virtualTCCR0B); // WGM02 cleared
}

TEST(EightBitTinyTimer0, SetCompareA) {
    virtualOCR0A = 0;
    
    timer0_set_ocra(42);
    
    BYTES_EQUAL(42, virtualOCR0A);
}

TEST(EightBitTinyTimer0, EnableOCRAInterrupt) {
    virtualTIMSK = 0;
    
    timer0_enable_ocra_interrupt();
    
    BYTES_EQUAL(B00010000, virtualTIMSK);
}

TEST(EightBitTinyTimer0, DisableOCRAInterrupt) {
    virtualTIMSK = 0xff;
    
    timer0_disable_ocra_interrupt();
    
    BYTES_EQUAL(B11101111, virtualTIMSK);
}
