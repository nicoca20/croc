
#include "util.h"
#include "config.h"
#include "adv_timer.h"

void timer0_init(int topvalue)
{
    // Reset the timer before configuring
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) = TIM_CMD_RST;

    // Select Clock: set TIM_CFG_SEL_CLK_SRC and mode = 0 to use low speed clock
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CFG) |= TIM_CFG_SEL_CLK_SRC;

    // Enable Clock of timer 0
    *reg32(ADV_TIMER_BASE_ADDR, REG_CH_EN) |= REG_TIM0_EN;

    // Set the bottom and top value of the counter
    // From where to where should be counted (both values are included).
    timer0_set_bottom_top_value(0, topvalue);

    // Start timer 0
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) |= TIM_CMD_START;
}

void timer0_pwm_init(int nCycles, int dutyCycle)
{
    // Reset the timer before configuring
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CMD) = TIM_CMD_RST;

    // Select Clock: set TIM_CFG_SEL_CLK_SRC and mode = 0 to use low speed clock (RTC)
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CFG) &= ~TIM_CFG_SEL_CLK_SRC;

    // Enable Clock of timer 0
    *reg32(ADV_TIMER_BASE_ADDR, REG_CH_EN) |= REG_TIM0_EN;

    // assert(dutyCycle >= 0 && dutyCycle <= 100);
    int toggle_val;
    if (dutyCycle >= 0 && dutyCycle <= 100)
    {
        toggle_val = (nCycles * dutyCycle) / 100;
    }
    else
    {
        int toggle_val = nCycles / 2;
    }
    // Set r_timer0_ch0_mode to OP_SETRST and set the threshold
    *reg32(ADV_TIMER_BASE_ADDR, REG_TIM0_CH0_TH) = (0b010 << 16) | toggle_val;

    // Set the bottom and top value of the counter
    // From where to where should be counted (both values are included).
    timer0_set_bottom_top_value(1, nCycles);

    // Start timer 0
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