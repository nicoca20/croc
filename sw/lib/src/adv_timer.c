
#include "util.h"
#include "config.h"
#include "adv_timer.h"

void timer0_init(int topvalue)
{
    // Reset the timer before configuring
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) = TIM_CMD_RST;

    // Stop the timer before configuring
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) |= TIM_CMD_STOP;

    // Configure the timer (Enable)
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CFG) |= TIM_CFG_SEL_CLK_SRC;

    // Enable Clock of timer 0
    *reg32(ADV_TIMER_BASE_ADDR, REG_CH_EN) |= REG_TIM0_EN;

    // Set the bottom and top value of the counter
    timer0_set_bottom_top_value(0, topvalue);

    // Start the timer
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) |= TIM_CMD_START;
}

int timer0_get_counter()
{
    return *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_COUNTER);
}

int timer0_get_top_value()
{
    return (*reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_TH) >> 16);
}

int timer0_get_bottom_value()
{
    return *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_TH) & 0xFFFF;
}

void timer0_set_bottom_top_value(int bottomvalue, int topvalue)
{
    int regvalue = ((topvalue & 0xFFFF) << 16) | (bottomvalue & 0xFFFF);
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_TH) = regvalue;
}