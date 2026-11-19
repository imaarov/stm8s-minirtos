#include <stdint.h>
#include "stm8s_regs.h"
#include "rtos.h"

#define LED_HEARTBEAT       (1u << 3)
#define LED_BUTTON          (1u << 4)
#define LED_HEAVY           (1u << 5)
#define BUTTON              (1u << 6)

#define LED_ALL             (LED_HEARTBEAT | LED_BUTTON | LED_HEAVY)

#define HEARTBEAT_PERIOD_MS 100u
#define BUTTON_POLL_MS      5u
#define BUTTON_DEBOUNCE_MS  30u
#define HEAVY_PERIOD_MS     4000u
#define HEAVY_DURATION_MS   2000u

#define TASK_STACK_BYTES    80u

static uint8_t stack_button[TASK_STACK_BYTES];
static uint8_t stack_heartbeat[TASK_STACK_BYTES];
static uint8_t stack_heavy[TASK_STACK_BYTES];

void tim4_isr(void) __naked __interrupt(IRQ_TIM4_UPD_OVF)
{
    __asm__(
        "ldw x, sp        \n" // save the stack pointer as quickly as we can
        "ldw _os_sp, x    \n" // save the "interrupted function (task)" stack pointer to the os_sp variable in the rtos file
        "call _os_tick    \n" // call the os_tick function of rtos to choose the new stack pointer
        "ldw x, _os_sp    \n" // the os_tick chose the winner stack pointer and save it to the os_sp variable
        "ldw sp, x        \n" // SWITCH stack from interrupted function "task" to the winner stack
        "iret             \n" // iret will pop the saved register and update the cpu reg's with the winner stack and continue running that function "task" execution from where its left
    );
}

inline static void led_on(uint8_t mask)
{
    PC_ODR |= mask;
}

inline static void led_off(uint8_t mask)
{
    PC_ODR &= (uint8_t)~mask;
}

inline static void led_toggle(uint8_t mask)
{
    PC_ODR ^= mask;
}

static void board_init(void)
{
    CLK_CKDIVR = 0x00;

    PC_DDR |= LED_ALL;
    PC_CR1 |= LED_ALL | BUTTON;

    led_off(LED_ALL);
}

inline static uint16_t elapsed(uint16_t since, uint16_t now)
{
    return (uint16_t)(now - since);
}

static void busy_wait_ms(uint16_t duration)
{
    uint16_t start = os_ms();

    while (elapsed(start, os_ms()) < duration) {
    }
}

static void task_button(void)
{
    uint8_t stable_released    = (PC_IDR & BUTTON) != 0;
    uint8_t candidate_released = stable_released;

    uint16_t last_sample     = os_ms();
    uint16_t candidate_since = last_sample;

    while (1) {
        uint16_t now;
        uint8_t released;

        os_sleep_ms(BUTTON_POLL_MS);

        now = os_ms();

        if (elapsed(last_sample, now) < BUTTON_POLL_MS) {
            continue;
        }

        last_sample = now;
        released    = (PC_IDR & BUTTON) != 0;

        if (released != candidate_released) {
            candidate_released = released;
            candidate_since    = now;
        }

        if (candidate_released != stable_released &&
            elapsed(candidate_since, now) >= BUTTON_DEBOUNCE_MS) {

            stable_released = candidate_released;

            if (!stable_released) {
                led_toggle(LED_BUTTON);
            }
        }
    }
}

static void task_heartbeat(void)
{
    uint16_t last_toggle = 0;

    while (1) {
        uint16_t now        = os_ms();
        uint16_t elapsed_ms = elapsed(last_toggle, now);
        uint16_t periods    = elapsed_ms / HEARTBEAT_PERIOD_MS;

        if (periods != 0) {
            last_toggle += (uint16_t)(periods * HEARTBEAT_PERIOD_MS);

            if (periods & 1u) {
                led_toggle(LED_HEARTBEAT);
            }
        }

        os_sleep_ms(HEARTBEAT_PERIOD_MS -
                    (elapsed(last_toggle, now) % HEARTBEAT_PERIOD_MS));
    }
}

static void task_heavy_job(void)
{
    os_sleep_ms(HEAVY_PERIOD_MS - HEAVY_DURATION_MS);

    while (1) {
        led_on(LED_HEAVY);
        busy_wait_ms(HEAVY_DURATION_MS);
        led_off(LED_HEAVY);

        os_sleep_ms(HEAVY_PERIOD_MS - HEAVY_DURATION_MS);
    }
}

void main(void)
{
    board_init();
    os_init();

    os_create_task(task_button, stack_button, sizeof stack_button);
    os_create_task(task_heartbeat, stack_heartbeat, sizeof stack_heartbeat);
    os_create_task(task_heavy_job, stack_heavy, sizeof stack_heavy);

    os_start();
}
