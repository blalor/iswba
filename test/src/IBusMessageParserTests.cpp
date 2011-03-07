extern "C" {
    #include "ibus_message_parser.h"
    #include "IBusMessageParserSpy.h"
}

#include <stdint.h>
#include "CppUTest/TestHarness.h"

TEST_GROUP(IBusMessageParser) {
    void setup() {
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

TEST(IBusMessageParser, DispatchInvalidMessage) {
    // feed an invalid message to the parser; bad checksum
    uint8_t test_msg[] = {0x80, 0x05, 0xBF, 0x18, 0x00, 0x00, 0x23};
    
    for (uint8_t i = 0; i < (sizeof(test_msg)/sizeof(test_msg[0])); i++) {
        message_parser_process_byte(test_msg[i]);
    }
    
    BYTES_EQUAL(0, get_handled_message_count());
    CHECK(get_handled_message() == NULL);
}
