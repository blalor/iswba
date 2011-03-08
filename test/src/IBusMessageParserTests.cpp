extern "C" {
    #include "ibus_message_parser.h"
    #include "IBusMessageParserSpy.h"
    #include "8bit_tiny_timer0.h"
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

TEST_GROUP(IBusMessageParser) {
    void setup() {
        virtualGTCCR = 0;
        virtualTCCR0A = 0;
        virtualTCCR0B = 0;
        virtualOCR0A = 0;
        virtualTIMSK = 0;
        virtualTCNT0 = 0;
        
        timer0_init(&timer0Regs, TIMER0_PRESCALE_1024);
        message_parser_init(&spy_handle_message);

        init_spy();
        CHECK(get_handled_message() == NULL);
    }
    
    void teardown() {
        // this is a hack, but forces the parser to free memory allocated
        // for the message
        message_parser_init(NULL);
    }
};

TEST(IBusMessageParser, DispatchValidMessage) {
    // feed a valid message to the parser
    uint8_t test_msg[] = {0x80, 0x05, 0xBF, 0x18, 0x00, 0x00, 0x22};
    
    for (uint8_t i = 0; i < (sizeof(test_msg)/sizeof(test_msg[0])); i++) {
        message_parser_process_byte(test_msg[i]);
    }
    
    const IBusMessage *parsed_message = get_handled_message();
    
    BYTES_EQUAL(1, get_handled_message_count());
    CHECK(parsed_message != NULL);
    CHECK(parsed_message->data != NULL);
    BYTES_EQUAL(3, parsed_message->data_length);
    
    for (uint8_t i = 0; i < parsed_message->data_length; i++) {
        BYTES_EQUAL(test_msg[i + 3], parsed_message->data[i]);
    }
}

/*
 * Verifies that the message parser resets the timer with every byte received.
 */
TEST(IBusMessageParser, CheckTimerReset) {
    virtualTCNT0 = 21;
    message_parser_process_byte(0x42);
    BYTES_EQUAL(0, virtualTCNT0);
}

TEST(IBusMessageParser, DispatchInvalidMessage) {
    // feed an invalid message to the parser; bad checksum
    uint8_t test_msg[] = {0x80, 0x05, 0xBF, 0x18, 0x00, 0x00, 0x23};
    
    for (uint8_t i = 0; i < (sizeof(test_msg)/sizeof(test_msg[0])); i++) {
        message_parser_process_byte(test_msg[i]);
    }
    
    BYTES_EQUAL(0, get_handled_message_count());
    CHECK(get_handled_message() == NULL);
}
