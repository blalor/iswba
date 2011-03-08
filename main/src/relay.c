#include "relay.h"

#include <avr/io.h>

static uint8_t pin_num;
static volatile uint8_t *port;

void relay_init(volatile uint8_t *_ddr, volatile uint8_t *_port, const uint8_t _pin_num) {
    pin_num = _pin_num;
    port = _port;
    
    *_ddr |= _BV(pin_num);
    
    relay_power_off();
}

void relay_power_off() {
    *port &= ~_BV(pin_num);
}

void relay_power_on() {
    *port |= _BV(pin_num);
}
