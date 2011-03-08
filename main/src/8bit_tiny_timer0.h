#ifndef EIGHT_BIT_TINY_TIMER
#define EIGHT_BIT_TINY_TIMER

#include <stdint.h>

typedef struct __timer0_regs {
    uint8_t *GTCCR;
    uint8_t *TCCRxA;
    uint8_t *TCCRxB;
    uint8_t *OCRxA;
    uint8_t *TIMSK;
    uint8_t *TCNTx;
} Timer0Registers;

typedef enum __prescalers {
    TIMER0_PRESCALE_1 =    ((0 << 2) | (0 << 1) | (1 << 0)),
    TIMER0_PRESCALE_8 =    ((0 << 2) | (1 << 1) | (0 << 0)),
    TIMER0_PRESCALE_64 =   ((0 << 2) | (1 << 1) | (1 << 0)),
    TIMER0_PRESCALE_256 =  ((1 << 2) | (0 << 1) | (0 << 0)),
    TIMER0_PRESCALE_1024 = ((1 << 2) | (0 << 1) | (1 << 0))
} Timer0Prescale;

void timer0_init(const Timer0Registers *regs, const Timer0Prescale prescale);

void timer0_attach_interrupt_ocra(const uint8_t counter_val, void (*handler)(void));
void timer0_reset(void);

void timer0_start(void);

#endif