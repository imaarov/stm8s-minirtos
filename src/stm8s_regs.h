#ifndef STM8S_REGS_H
#define STM8S_REGS_H

#include <stdint.h>

#define REG8(addr) (*(volatile uint8_t *)(addr))

/* Clock */
#define CLK_CKDIVR REG8(0x50C6)   /* 0x00 = HSI 16 MHz, CPU undivided */

/* GPIOC: base 0x500A, offsets ODR +0, IDR +1, DDR +2, CR1 +3, CR2 +4 */
#define PC_ODR REG8(0x500A)
#define PC_IDR REG8(0x500B)
#define PC_DDR REG8(0x500C)
#define PC_CR1 REG8(0x500D)
#define PC_CR2 REG8(0x500E)

/* TIM4: 8-bit basic timer */
#define TIM4_CR1  REG8(0x5340)
#define TIM4_IER  REG8(0x5343)
#define TIM4_SR   REG8(0x5344)
#define TIM4_EGR  REG8(0x5345)
#define TIM4_PSCR REG8(0x5347)
#define TIM4_ARR  REG8(0x5348)

#define TIM4_CR1_CEN (1u << 0)    /* counter enable */
#define TIM4_IER_UIE (1u << 0)    /* update interrupt enable */
#define TIM4_SR_UIF  (1u << 0)    /* update flag (rc_w0: write 0 to clear) */
#define TIM4_EGR_UG  (1u << 0)    /* force an update event */

/* interrupt vector numbers (for SDCC __interrupt(n)) */
#define IRQ_TIM4_UPD_OVF 23

#endif
