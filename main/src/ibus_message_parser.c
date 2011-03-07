#include "ibus_message_parser.h"

#include <string.h>

// reference to the message handler
static void (*message_handler)(const uint8_t *msg, uint8_t msg_len);

// temporary buffer for storing the incoming message
static uint8_t message_buffer[256];

// index into message_buffer
static uint8_t msg_buf_ind;

void message_parser_init(void (*_message_handler)(const uint8_t *msg, const uint8_t msg_len)) {
    message_handler = _message_handler;
    
    msg_buf_ind = 0;
}

void message_parser_process_byte(const uint8_t _byte) {
    message_buffer[msg_buf_ind++] = _byte;
    
    message_handler(message_buffer, msg_buf_ind);
}
