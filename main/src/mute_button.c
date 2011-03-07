#include "mute_button.h"

static volatile uint8_t *port;
static uint8_t pin_num;

void mute_button_init(volatile uint8_t *_ddr, volatile uint8_t *_port, const uint8_t _pin_num) {
    pin_num = _pin_num;
    port = _port;
    
    *_ddr |= (1 << pin_num);
    
    mute_button_release();
}

void mute_button_press() {
    *port |= (1 << pin_num);
}

void mute_button_release() {
    *port &= ~(1 << pin_num);
}
