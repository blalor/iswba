#ifndef IBUS_MESSAGE_PARSER_H
#define IBUS_MESSAGE_PARSER_H

#include <stdint.h>

typedef struct __ibus_msg {
    uint8_t source;
    uint8_t destination;
    uint8_t data_length;
    uint8_t *data;
} IBusMessage;

/*
 * Initializes the message parser.
 *
 * @param message_handler callback method invoked when a valid message has been received.
 */
void message_parser_init(void (*message_handler)(const IBusMessage *msg));

/*
 * Consumes a byte from the serial device.
 *
 * @param _byte the byte received
 */
void message_parser_process_byte(const uint8_t _byte);

/*
 * Resets the parser state. Passed to timer0_attach_interrupt_ocra() (or 
 * similar) to implement message reception timeout.
 */
void message_parser_reset(void);

#endif
