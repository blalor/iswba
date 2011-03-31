#ifndef IBUS_BUTTON_ADAPTER_H
#define IBUS_BUTTON_ADAPTER_H

#include "ibus_message_parser.h"

void button_adapter_init(void);
void button_adapter_handle_message(const volatile IBusMessage *msg);

#endif
