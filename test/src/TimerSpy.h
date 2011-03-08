#ifndef TIMER_SPY_H
#define TIMER_SPY_H

#include <stdint.h>

void timer_spy_reset(void);
uint8_t timer_spy_get_interrupt_count(void);

void timer_spy_handle_interrupt(void);

#endif
