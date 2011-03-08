#ifndef EIGHT_BIT_TINY_TIMER
#define EIGHT_BIT_TINY_TIMER

#include <avr/io.h>
#include <stdint.h>

typedef struct __timer0_regs {
    volatile uint8_t *pGTCCR;
    volatile uint8_t *pTCCR0A;
    volatile uint8_t *pTCCR0B;
    volatile uint8_t *pOCR0A;
    volatile uint8_t *pTIMSK;
    volatile uint8_t *pTCNT0;
} Timer0Registers;

typedef enum __prescalers {
    TIMER0_PRESCALE_1 =                            _BV(CS00),
    TIMER0_PRESCALE_8 =                _BV(CS01)            ,
    TIMER0_PRESCALE_64 =               _BV(CS01) | _BV(CS00),
    TIMER0_PRESCALE_256 =  _BV(CS02)                        ,
    TIMER0_PRESCALE_1024 = _BV(CS02) |             _BV(CS00),
} Timer0Prescale;

void timer0_init(const Timer0Registers *regs, const Timer0Prescale prescale);

void timer0_attach_interrupt_ocra(const uint8_t counter_val, void (*handler)(void));
void timer0_reset(void);

void timer0_start(void);

#endif