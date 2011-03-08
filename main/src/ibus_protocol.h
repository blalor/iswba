#ifndef IBUS_PROTOCOL_H
#define IBUS_PROTOCOL_H

#include <stdint.h>

typedef enum __addrs {
    IBUS_ADDR_MFL = 0x50,
    IBUS_ADDR_RAD = 0x68,
} IBusAddress;

typedef struct __ibus_msg {
    IBusAddress source;
    IBusAddress destination;
    uint8_t data_length;
    uint8_t *data;
} IBusMessage;

#endif
