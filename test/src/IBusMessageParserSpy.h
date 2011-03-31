/*
 * Spy module for intercepting messages processed by the IBusMessageParser.
 */

#ifndef IBUS_MESSAGE_PARSER_SPY_H
#define IBUS_MESSAGE_PARSER_SPY_H

#include "ibus_message_parser.h"
#include <stdint.h>

void init_spy(void);

const IBusMessage *get_handled_message(void);
uint8_t get_handled_message_count(void);

void spy_handle_message(volatile const IBusMessage *msg);

#endif
