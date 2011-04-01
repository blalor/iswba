#include "ibus_message_parser.h"

#include <stdlib.h>

typedef struct __parser_state {
    // index into message_buffer
    uint8_t buffer_ind;
    
    // pending checksum
    uint8_t checksum;
    
    // pointer to ibus message data
    uint8_t *data_ptr;
} ParserState;

static volatile ParserState parserState;
static volatile IBusMessage ibusMessage;

// reference to the message handler
static void (*message_handler)(const volatile IBusMessage *msg);

// reference to the timer set method
static void (*timer_set_counter)(const uint8_t counter_val);

void message_parser_reset(void) {
    if (parserState.buffer_ind != 0) {
        ibusMessage.source = 0;
        ibusMessage.destination = 0;
        ibusMessage.data_length = 0;

        if (ibusMessage.data != NULL) {
            free(ibusMessage.data);
        }

        ibusMessage.data = NULL;

        parserState.buffer_ind = 0;
        parserState.checksum = 0;
    }
}

void message_parser_init(
    void (*_message_handler)(volatile const IBusMessage *msg),
    void (*_timer_set_counter)(const uint8_t timer_val)
) {
    message_handler = _message_handler;
    timer_set_counter = _timer_set_counter;

    message_parser_reset();
}

// {{{ message_parser_process_byte
void message_parser_process_byte(const uint8_t _byte) {
    // reset timer to indicate activity
    timer_set_counter(0);
    
    if (parserState.buffer_ind < (ibusMessage.data_length + 3)) {
        if (parserState.buffer_ind == 0) {
            ibusMessage.source = _byte;
        }
        else if (parserState.buffer_ind == 1) {
            // store the length of the actual data, not of the data, checksum,
            // and destination address.
            ibusMessage.data_length = _byte - 2;
            ibusMessage.data = (uint8_t *) malloc(ibusMessage.data_length);

            parserState.data_ptr = ibusMessage.data;
        }
        else if (parserState.buffer_ind == 2) {
            ibusMessage.destination = _byte;
        }
        else {
            *parserState.data_ptr++ = _byte;
        }
        
        parserState.checksum ^= _byte;
        parserState.buffer_ind += 1;
    }
    else {
        // this is the checksum
        if (_byte == parserState.checksum) {
            // valid message
            message_handler(&ibusMessage);
        }
        
        // always reset the parser at the end of a message
        message_parser_reset();
    }
}
// }}}
