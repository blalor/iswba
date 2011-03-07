#ifndef RELAY_H
#define RELAY_H

#include <stdint.h>

/*
 * Initializes the relay.
 *
 * @param _port    PORTx register
 * @param _ddr     DDRx register
 * @param _pin_num pin number
 */
void relay_init(volatile uint8_t *_ddr, volatile uint8_t *_port, const uint8_t _pin_num);

/* Closes relay. */
void relay_power_on(void);

/* Opens relay. */
void relay_power_off(void);

#endif
