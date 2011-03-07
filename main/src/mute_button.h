#ifndef MUTE_BUTTON_H
#define MUTE_BUTTON_H

#include <stdint.h>

/*
 * Initializes the mute button hardware.
 *
 * @param _port    PORTx register
 * @param _ddr     DDRx register
 * @param _pin_num pin number
 */
void mute_button_init(volatile uint8_t *_ddr, volatile uint8_t *_port, const uint8_t _pin_num);

/*
 * "Presses" the mute button.
 */
void mute_button_press(void);

/*
 * "Releases" the mute button.
 */
void mute_button_release(void);

#endif
