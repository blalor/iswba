#ifndef EIGHT_BIT_TINY_TIMER
#define EIGHT_BIT_TINY_TIMER

#include <stdint.h>

typedef struct __timer0_regs {
    volatile uint8_t *pGTCCR;
    volatile uint8_t *pTCCRxA;
    volatile uint8_t *pTCCRxB;
    volatile uint8_t *pOCRxA;
    volatile uint8_t *pTIMSK;
    volatile uint8_t *pTCNTx;
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