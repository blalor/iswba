#include "TimerSpy.h"

static uint8_t interrupt_count;

void timer_spy_reset(void) {
    interrupt_count = 0;
}

uint8_t timer_spy_get_interrupt_count(void) {
    return interrupt_count;
}

void timer_spy_handle_interrupt() {
    interrupt_count += 1;
}
