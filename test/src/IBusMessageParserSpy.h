/*
 * Spy module for intercepting messages processed by the IBusMessageParser.
 */

#ifndef IBUS_MESSAGE_PARSER_SPY_H
#define IBUS_MESSAGE_PARSER_SPY_H

#include <stdint.h>

void init_buffer(void);
const uint8_t *get_buffer(void);
uint8_t get_buffer_length(void);

void handle_message(const uint8_t *msg, const uint8_t msg_len);

#endif
