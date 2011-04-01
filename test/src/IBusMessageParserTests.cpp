extern "C" {
    #include "ibus_message_parser.h"
    #include "IBusMessageParserSpy.h"
    #include "8bit_tiny_timer0.h"

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

TEST_GROUP(IBusMessageParser) {
    void setup() {
        virtualGTCCR = 0;
        virtualTCCR0A = 0;
        virtualTCCR0B = 0;
        virtualOCR0A = 0;
        virtualTIMSK = 0;
        virtualTIFR = 0;
        virtualTCNT0 = 0;
        
        /*
        Timer duration is a factor of the prescaler and CPU frequency. It must
        be calculated by hand and hard-coded into the application. These are
        just dummy values.
        */
        timer0_init(&timer0Regs, TIMER0_PRESCALE_1024);
        timer0_set_ocra_interrupt_handler(&message_parser_reset);
        message_parser_init(&spy_handle_message, &timer0_set_counter);
        timer0_start();

        init_spy();
        CHECK(get_handled_message() == NULL);
    }
    
    void teardown() {
        // this is a hack, but forces the parser to free memory allocated
        // for the message
        message_parser_init(NULL, NULL);
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
    CHECK(parsed_message->source == 0x80);
    CHECK(parsed_message->destination == 0xBF);
    
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

/*
 * Verifies that the parser correctly discards any pending data when the timer
 * expires, indicating that message transmission has finished, regardless of
 * whether the in-progress message was complete or correct.
 */
TEST(IBusMessageParser, ParserResetAfterMessageTimeout) {
    // feed partial garbage data
    uint8_t test_msg1[] = {0x11, 0x22, 0x33};

    for (uint8_t i = 0; i < (sizeof(test_msg1)/sizeof(test_msg1[0])); i++) {
        message_parser_process_byte(test_msg1[i]);
    }
    
    // trigger the "interrupt" as if the timer had expired
    ISR_TIMER0_COMPA_vect();
    
    // now the real message
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

TEST(IBusMessageParser, ParseTwoConsecutiveValidMessages) {
    uint8_t test_msgs[][7] = {
        {0x80, 0x05, 0xBF, 0x18, 0x00, 0x00, 0x22},
        {0x80, 0x05, 0xBF, 0x18, 0x00, 0x00, 0x22}
    };
    
    for (uint8_t i = 0; i < (sizeof(test_msgs)/sizeof(test_msgs[0])); i++) {
        for (uint8_t j = 0; j < (sizeof(test_msgs[i])/sizeof(test_msgs[i][0])); j++) {
            message_parser_process_byte(test_msgs[i][j]);
        }
    }    
    
    BYTES_EQUAL(2, get_handled_message_count());
}

TEST(IBusMessageParser, ParseRTPress) {
    uint8_t test_msg[] = {0x50, 0x04, 0x68, 0x3B, 0x02, 0x05};
    
    for (uint8_t i = 0; i < (sizeof(test_msg)/sizeof(test_msg[0])); i++) {
        message_parser_process_byte(test_msg[i]);
    }
    
    BYTES_EQUAL(1, get_handled_message_count());

    const IBusMessage *parsed_message = get_handled_message();
    CHECK(parsed_message != NULL);
    CHECK(parsed_message->data != NULL);
    BYTES_EQUAL(2, parsed_message->data_length);
    
    for (uint8_t i = 0; i < parsed_message->data_length; i++) {
        BYTES_EQUAL(test_msg[i + 3], parsed_message->data[i]);
    }
}
