#include "IBusMessageParserSpy.h"
#include "ibus_message_parser.h"

#include <string.h>

static IBusMessage msg;
static const IBusMessage *pMsg;
static uint8_t handled_message_count;

void init_spy(void) {
    pMsg = 0;
    handled_message_count = 0;
}

const IBusMessage *get_handled_message() {
    return pMsg;
}

uint8_t get_handled_message_count(void) {
    return handled_message_count;
}

void spy_handle_message(volatile const IBusMessage *_msg) {
    // since the parser gets reset after the message is processed, we need to
    // keep a copy of the processed message to return to our handler
    memcpy((void *)&msg, (const void *)_msg, sizeof(IBusMessage));
    pMsg = &msg;
    
    handled_message_count += 1;
}

