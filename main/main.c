/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usi_serial.h"
#include "relay.h"
#include "mute_button.h"
#include "8bit_tiny_timer0.h"
#include "8bit_tiny_timer1.h"
#include "ibus_message_parser.h"
#include "ibus_button_adapter.h"

// compare value for OCR1A to determine when data transmission has stopped
// With integer math, this comes out to 66 instead of 72; going the precalc route
/*
#define SINGLE_BYTE_RX_TIMEOUT ( \
    (F_CPU/(BAUD_RATE*128)) * \
    (START_BITS + DATA_BITS + PARITY_BITS + STOP_BITS) \
)
*/
#define SINGLE_BYTE_RX_TIMEOUT 72

static const Timer0Registers timer0Regs = {
    &GTCCR,
    &TCCR0A,
    &TCCR0B,
    &OCR0A,
    &TIMSK,
    &TCNT0
};

static const Timer1Registers timer1Regs = {
    &GTCCR,
    &TCCR1,
    &OCR1A,
    &OCR1B,
    &TIMSK,
    &TCNT1,
};

static const USISerialRxRegisters usiRegs = {
    &PORTB,
    &PINB,
    &DDRB,
    &USIBR,
    &USICR,
    &USISR,
    &GIFR,
    &GIMSK,
    &PCMSK,
};

/*
                                   PDIP/SOIC/TSSOP
       (PCINT5/RESET/ADC0/dW) PB5                   VCC
(PCINT3/XTAL1/CLKI/OC1B/ADC3) PB3                   PB2 (SCK/USCK/SCL/ADC1/T0/INT0/PCINT2)
(PCINT4/XTAL2/CLKO/OC1B/ADC2) PB4                   PB1 (MISO/DO/AIN1/OC0B/OC1A/PCINT1)
                              GND                   PB0 (MOSI/DI/SDA/AIN0/OC0A/OC1A/AREF/PCINT0)
*/

int main(void) {
    relay_init(&DDRB, &PORTB, PB3);
    mute_button_init(&DDRB, &PORTB, PB4);
    button_adapter_init();

    // must initialize Timer0 first
    timer0_init(&timer0Regs, TIMER0_PRESCALE_8);
    timer1_init(&timer1Regs, TIMER1_PRESCALE_128);
    
    message_parser_init(&button_adapter_handle_message, &timer1_set_counter);
    timer1_attach_interrupt_ocra(SINGLE_BYTE_RX_TIMEOUT, &message_parser_reset);
    
    usi_serial_receiver_init(&usiRegs, &message_parser_process_byte);
    
    timer1_start();

    // and we're off!
    sei();
    
    for(;;){
        /* insert your main loop code here */
    }
    
    return 0;   /* never reached */
}
