#include "ibus_button_adapter.h"
#include "relay.h"
#include "mute_button.h"

#include "ibus_protocol.h"

#include <stdbool.h>

typedef struct __button_state {
    bool pressed;
    // bool pressed_long;
} ButtonState;

typedef enum __buttons {
    MFL_RT_PRESSED           = 0x02,
    // MFL_RT_PRESSED_LONG      = 0x12,
    MFL_RT_RELEASED          = 0x22,
    
    MFL_SENDEND_PRESSED      = 0x80,
    // MFL_SENDEND_PRESSED_LONG = 0x90,
    MFL_SENDEND_RELEASED     = 0xA0,
} Button;

static ButtonState rtState;
bool relay_on;

void button_adapter_init() {
    relay_power_on();
    relay_on = true;
    
    mute_button_release();
}

void button_adapter_handle_message(const volatile IBusMessage *msg) {
    // check for message from multi-function steering wheel to radio with
    // appropriate length and command byte
    if (
        (msg->source      == IBUS_ADDR_MFL) &&
        (msg->destination == IBUS_ADDR_RAD) &&
        (msg->data_length == 2) &&
        (msg->data[0] == 0x3B)
    ) {
        Button button = (Button) msg->data[1];
        
        switch (button) {
            case MFL_RT_PRESSED:
                rtState.pressed = true;
                break;

            // case MFL_RT_PRESSED_LONG:
            //     rtState.pressed_long = true;
            //     break;

            case MFL_RT_RELEASED:
                if (rtState.pressed) {
                    rtState.pressed = false;

                    if (relay_on) {
                        relay_power_off();
                        relay_on = false;
                    } else {
                        relay_power_on();
                        relay_on = true;
                    }
                }
                
                break;

            case MFL_SENDEND_PRESSED:
                mute_button_press();
                break;
            
            // case MFL_SENDEND_PRESSED_LONG:
            //     break;
            
            case MFL_SENDEND_RELEASED:
                mute_button_release();
                break;
            
            default:
                // not handled
                break;
        }
    }
}
