#include "IBusMessageParserSpy.h"
#include "ibus_message_parser.h"

static const IBusMessage *msg;
static uint8_t handled_message_count;

void init_spy(void) {
    msg = 0;
    handled_message_count = 0;
}

const IBusMessage *get_handled_message() {
    return msg;
}

uint8_t get_handled_message_count(void) {
    return handled_message_count;
}

void spy_handle_message(const IBusMessage *_msg) {
    msg = _msg;
    
    handled_message_count += 1;
}

