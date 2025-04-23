// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Author:
// - Nico Canzani <ncanzani@student.ethz.ch>

#ifndef __TIMER_REGS_H__
#define __TIMER_REGS_H__

// -----------------------------------------------------------------------------
// Timer Command Register bits
// -----------------------------------------------------------------------------
#define TIM_CMD_START (1 << 0)
#define TIM_CMD_STOP (1 << 1)
#define TIM_CMD_UPDATE (1 << 2)
#define TIM_CMD_RST (1 << 3)
#define TIM_CMD_ARM (1 << 4)

// -----------------------------------------------------------------------------
// Timer Configuration Register bits / Masks
// -----------------------------------------------------------------------------
#define TIM_CFG_IN_SEL_MASK (0xFF)
#define TIM_CFG_IN_MODE_MASK (0x7 << 8)
#define TIM_CFG_SEL_CLK_SRC (1 << 11)
#define TIM_CFG_SEL_SAW (1 << 12)
#define TIM_CFG_PRESC_MASK (0xFF << 16)

// -----------------------------------------------------------------------------
// Timer 0 Registers
// -----------------------------------------------------------------------------
#define REG_TIM0_CMD 0x000
#define REG_TIM0_CFG 0x004
#define REG_TIM0_TH 0x008
#define REG_TIM0_CH0_TH 0x00C
#define REG_TIM0_CH1_TH 0x010
#define REG_TIM0_CH2_TH 0x014
#define REG_TIM0_CH3_TH 0x018
#define REG_TIM0_CH0_LUT 0x01C
#define REG_TIM0_CH1_LUT 0x020
#define REG_TIM0_CH2_LUT 0x024
#define REG_TIM0_CH3_LUT 0x028
#define REG_TIM0_COUNTER 0x02C

// -----------------------------------------------------------------------------
// Timer 1 Registers
// -----------------------------------------------------------------------------
#define REG_TIM1_CMD 0x040
#define REG_TIM1_CFG 0x044
#define REG_TIM1_TH 0x048
#define REG_TIM1_CH0_TH 0x04C
#define REG_TIM1_CH1_TH 0x050
#define REG_TIM1_CH2_TH 0x054
#define REG_TIM1_CH3_TH 0x058
#define REG_TIM1_CH0_LUT 0x05C
#define REG_TIM1_CH1_LUT 0x060
#define REG_TIM1_CH2_LUT 0x064
#define REG_TIM1_CH3_LUT 0x068
#define REG_TIM1_COUNTER 0x06C

// -----------------------------------------------------------------------------
// Timer 2 Registers
// -----------------------------------------------------------------------------
#define REG_TIM2_CMD 0x080
#define REG_TIM2_CFG 0x084
#define REG_TIM2_TH 0x088
#define REG_TIM2_CH0_TH 0x08C
#define REG_TIM2_CH1_TH 0x090
#define REG_TIM2_CH2_TH 0x094
#define REG_TIM2_CH3_TH 0x098
#define REG_TIM2_CH0_LUT 0x09C
#define REG_TIM2_CH1_LUT 0x0A0
#define REG_TIM2_CH2_LUT 0x0A4
#define REG_TIM2_CH3_LUT 0x0A8
#define REG_TIM2_COUNTER 0x0AC

// -----------------------------------------------------------------------------
// Timer 3 Registers
// -----------------------------------------------------------------------------
#define REG_TIM3_CMD 0x0C0
#define REG_TIM3_CFG 0x0C4
#define REG_TIM3_TH 0x0C8
#define REG_TIM3_CH0_TH 0x0CC
#define REG_TIM3_CH1_TH 0x0D0
#define REG_TIM3_CH2_TH 0x0D4
#define REG_TIM3_CH3_TH 0x0D8
#define REG_TIM3_CH0_LUT 0x0DC
#define REG_TIM3_CH1_LUT 0x0E0
#define REG_TIM3_CH2_LUT 0x0E4
#define REG_TIM3_CH3_LUT 0x0E8
#define REG_TIM3_COUNTER 0x0EC

// others
#define REG_EVENT_CFG 0x100
#define REG_CH_EN 0x104

#define REG_TIM0_EN (1 << 0)
#define REG_TIM1_EN (1 << 1)
#define REG_TIM2_EN (1 << 2)
#define REG_TIM3_EN (1 << 3)

void timer0_init(int topvalue);

void timer0_pwm_init(int nCycles, int dutycycle_value);

int timer0_get_counter();

int timer0_get_top_value();
int timer0_get_bottom_value();
void timer0_set_bottom_top_value(int bottomvalue, int topvalue);

#endif // __TIMER_REGS_H__
