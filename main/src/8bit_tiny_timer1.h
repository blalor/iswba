#ifndef EIGHT_BIT_TINY_TIMER1_H
#define EIGHT_BIT_TINY_TIMER1_H

#include <avr/io.h>
#include <stdint.h>

typedef struct __timer1_regs {
    volatile uint8_t *pGTCCR;
    volatile uint8_t *pTCCR1;
    volatile uint8_t *pOCR1A;
    volatile uint8_t *pOCR1B;
    volatile uint8_t *pTIMSK;
    volatile uint8_t *pTCNT1;
} Timer1Registers;

typedef enum __prescalers1 {
    TIMER1_PRESCALE_1024 = _BV(CS13)             | _BV(CS11) | _BV(CS10),
} Timer1Prescale;

void timer1_init(const Timer1Registers *regs, const Timer1Prescale prescale);

/*
 * Install the interrupt handler for OCR1A and set the increment value.
 */
void timer1_attach_interrupt_ocra(void (*handler)(void), const uint8_t incr_val);

/*
 * Install the interrupt handler for OCR1B and set the increment value.
 */
void timer1_attach_interrupt_ocrb(void (*handler)(void), const uint8_t incr_val);

/*
 * Increment the value of OCR1A.
 */
void timer1_reset_ocra(void);

/*
 * Increment the value of OCR1B.
 */
void timer1_reset_ocrb(void);

#endif