extern "C" {
    #include "ibus_message_parser.h"
    #include "IBusMessageParserSpy.h"
}

#include <stdint.h>
#include "CppUTest/TestHarness.h"

TEST_GROUP(IBusMessageParser) {
    void setup() {
        init_buffer();
        CHECK(get_buffer() == NULL);
    }
};

TEST(IBusMessageParser, DispatchValidMessage) {
    message_parser_init(&handle_message);
    
    // feed a valid message to the parser
    uint8_t test_msg[] = {0x80, 0x05, 0xBF, 0x18, 0x00, 0x00, 0x22};
    
    for (uint8_t i = 0; i < (sizeof(test_msg)/sizeof(test_msg[0])); i++) {
        message_parser_process_byte(test_msg[i]);
    }
    
    const uint8_t *parsed_message = get_buffer();
    const uint8_t message_length = get_buffer_length();
    
    CHECK(parsed_message != NULL);
    BYTES_EQUAL((sizeof(test_msg)/sizeof(test_msg[0])), message_length);
    
    for (uint8_t i = 0; i < message_length; i++) {
        BYTES_EQUAL(test_msg[i], parsed_message[i]);
    }
}
