#ifndef IBUS_MESSAGE_PARSER_H
#define IBUS_MESSAGE_PARSER_H

#include "ibus_protocol.h"

#include <stdint.h>

/*
 * Initializes the message parser.
 *
 * @param message_handler callback method invoked when a valid message has been received.
 */
void message_parser_init(
    void (*message_handler)(const IBusMessage *msg),
    void (*timer_reset)(void)
);

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
