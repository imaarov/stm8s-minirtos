#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>

#define OS_MAX_TASKS 4

typedef void (*os_task_fn)(void);

void os_init(void);

void os_create_task(os_task_fn fn,
                    uint8_t *stack,
                    uint8_t stack_size);

void os_start(void);

uint16_t os_ms(void);

void os_sleep_ms(uint16_t ms);

extern volatile uint16_t os_sp;

void os_tick(void);

#endif
