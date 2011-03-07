#ifndef IBUS_MESSAGE_PARSER_H
#define IBUS_MESSAGE_PARSER_H

#include <stdint.h>

/*
 * Initializes the message parser.
 *
 * @param message_handler callback method invoked when a valid message has been received.
 */
void message_parser_init(void (*message_handler)(const uint8_t *msg, const uint8_t msg_len));

/*
 * Consumes a byte from the serial device.
 *
 * @param _byte the byte received
 */
void message_parser_process_byte(const uint8_t _byte);

#endif
