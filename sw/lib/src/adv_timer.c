
#include "util.h"
#include "config.h"
#include "adv_timer.h"

void timer0_init(uint32_t threshold)
{
    // Reset the timer before configuring
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) = TIM_CMD_RST;

    // Stop the timer before configuring
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) |= TIM_CMD_STOP;

    // Configure the timer (Enable)
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CFG) &= ~TIM_CFG_SEL_CLK_SRC;

    // Enable Clock of timer 0
    *reg32(ADV_TIMER_BASE_ADDR, REG_CH_EN) = (1 << 0);

    // Set the threshold value
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_TH) = threshold;

    // Start the timer
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) |= TIM_CMD_START;
}

uint32_t timer0_get_counter()
{
    return *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_COUNTER);
}

uint32_t timer0_get_threshold()
{
    return *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_TH);
}
