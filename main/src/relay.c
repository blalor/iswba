#include "relay.h"

static uint8_t pin_num;
static volatile uint8_t *port;

void relay_init(volatile uint8_t *_ddr, volatile uint8_t *_port, const uint8_t _pin_num) {
    pin_num = _pin_num;
    port = _port;
    
    *_ddr |= (1 << pin_num);
    
    relay_power_off();
}

void relay_power_off() {
    *port &= ~(1 << pin_num);
}

void relay_power_on() {
    *port |= (1 << pin_num);
}
