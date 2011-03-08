/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>

#include "relay.h"
#include "mute_button.h"
#include "8bit_tiny_timer0.h"
#include "ibus_message_parser.h"

static const Timer0Registers timer0Regs = {
    &GTCCR,
    &TCCR0A,
    &TCCR0B,
    &OCR0A,
    &TIMSK,
    &TCNT0
};

void handle_message(const IBusMessage *msg) {
    
}

int main(void) {
    relay_init(&DDRB, &PORTB, 3);
    mute_button_init(&DDRB, &PORTB, 4);
    
    timer0_init(&timer0Regs, TIMER0_PRESCALE_1024);
    timer0_attach_interrupt_ocra(42, &message_parser_reset);
    message_parser_init(&handle_message);
    
    
    for(;;){
        /* insert your main loop code here */
    }
    return 0;   /* never reached */
}
