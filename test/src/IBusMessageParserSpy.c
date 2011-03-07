#include "IBusMessageParserSpy.h"

static const uint8_t *buffer;
static uint8_t msg_len;

void init_buffer(void) {
    buffer = 0;
}

const uint8_t *get_buffer() {
    return buffer;
}

uint8_t get_buffer_length() {
    return msg_len;
}

void handle_message(const uint8_t *msg, uint8_t _msg_len) {
    buffer = msg;
    msg_len = _msg_len;
}

