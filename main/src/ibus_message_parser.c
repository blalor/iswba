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

static ParserState parserState;
static IBusMessage ibusMessage;

// reference to the message handler
static void (*message_handler)(const IBusMessage *msg);

// reference to the timer reset method
static void (*timer_reset)(void);

void message_parser_reset(void) {
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

void message_parser_init(
    void (*_message_handler)(const IBusMessage *msg),
    void (*_timer_reset)(void)
) {
    message_handler = _message_handler;
    timer_reset = _timer_reset;

    message_parser_reset();
}

// {{{ message_parser_process_byte
void message_parser_process_byte(const uint8_t _byte) {
    // reset timer to indicate activity
    timer_reset();
    
    if (parserState.buffer_ind == 0) {
        ibusMessage.source = _byte;

        parserState.checksum ^= _byte;
        parserState.buffer_ind += 1;
    }
    else if (parserState.buffer_ind == 1) {
        // store the length of the actual data, not of the data, checksum,
        // and destination address.
        ibusMessage.data_length = _byte - 2;
        ibusMessage.data = (uint8_t *) malloc(ibusMessage.data_length);
    
        parserState.data_ptr = ibusMessage.data;
        
        parserState.checksum ^= _byte;
        parserState.buffer_ind += 1;
    }
    else if (parserState.buffer_ind == 2) {
        ibusMessage.destination = _byte;

        parserState.checksum ^= _byte;
        parserState.buffer_ind += 1;
    }
    else if (parserState.buffer_ind < (ibusMessage.data_length + 3)) {
        *parserState.data_ptr++ = _byte;
        
        parserState.checksum ^= _byte;
        parserState.buffer_ind += 1;
    }
    else {
        // this is the checksum
        if (_byte == parserState.checksum) {
            // valid message
            message_handler(&ibusMessage);
        } else {
            // checksum mismatch
            message_parser_reset();
        } 
    }
}
// }}}
