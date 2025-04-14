// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Author:
// - Nico Canzani <ncanzani@student.ethz.ch>

#ifndef __TIMER_REGS_H__
#define __TIMER_REGS_H__

#define ADV_TIMER_BASE_ADDR 0x20001000

// Timer 0 Register Offsets
#define REG_TIM0_CMD 0x000     // Command Register
#define REG_TIM0_CFG 0x004     // Configuration Register
#define REG_TIM0_TH 0x008      // Threshold Register
#define REG_TIM0_COUNTER 0x02C // Current Timer Counter

// -----------------------------------------------------------------------------
// Timer Command Register (REG_TIM0_CMD) bits
// -----------------------------------------------------------------------------
#define TIM_CMD_START (1 << 0)
#define TIM_CMD_STOP (1 << 1)
#define TIM_CMD_UPDATE (1 << 2)
#define TIM_CMD_RST (1 << 3)
#define TIM_CMD_ARM (1 << 4)

// -----------------------------------------------------------------------------
// Timer Configuration Register (REG_TIM0_CFG) bits
// -----------------------------------------------------------------------------
#define TIM_CFG_IN_SEL_MASK (0xFF)      // Bits [7:0]: Input Select
#define TIM_CFG_IN_MODE_MASK (0x7 << 8) // Bits [10:8]: Input Mode
#define TIM_CFG_SEL_CLK_SRC (1 << 11)   // Bit [11]: Clock Source Select
#define TIM_CFG_SEL_SAW (1 << 12)       // Bit [12]: Sawtooth Mode
#define TIM_CFG_PRESC_MASK (0xFF << 16) // Bits [23:16]: Prescaler

// -----------------------------------------------------------------------------
// Timer Channel 0 Threshold Register (REG_TIM0_CH0_TH) bits
// -----------------------------------------------------------------------------
#define TIM_CH0_TH_MASK (0xFFFF)      // Bits [15:0]: Channel 0 Threshold
#define TIM_CH0_MODE_MASK (0x7 << 16) // Bits [18:16]: Channel 0 Mode

// Timer Channel 0 LUT Register (REG_TIM0_CH0_LUT) bits
#define TIM_CH0_LUT_MASK (0xFFFF)    // Bits [15:0]: Channel 0 LUT
#define TIM_CH0_FLT_MASK (0x3 << 16) // Bits [17:16]: Channel 0 Filter

// -----------------------------------------------------------------------------
// Timer Channel 1 Threshold Register (REG_TIM0_CH1_TH) bits
// -----------------------------------------------------------------------------
#define TIM_CH1_TH_MASK (0xFFFF)      // Bits [15:0]: Channel 1 Threshold
#define TIM_CH1_MODE_MASK (0x7 << 16) // Bits [18:16]: Channel 1 Mode

// Timer Channel 1 LUT Register (REG_TIM0_CH1_LUT) bits
#define TIM_CH1_LUT_MASK (0xFFFF)    // Bits [15:0]: Channel 1 LUT
#define TIM_CH1_FLT_MASK (0x3 << 16) // Bits [17:16]: Channel 1 Filter

// -----------------------------------------------------------------------------
// Timer Channel 2 Threshold Register (REG_TIM0_CH2_TH) bits
// -----------------------------------------------------------------------------
#define TIM_CH2_TH_MASK (0xFFFF)      // Bits [15:0]: Channel 2 Threshold
#define TIM_CH2_MODE_MASK (0x7 << 16) // Bits [18:16]: Channel 2 Mode

// Timer Channel 2 LUT Register (REG_TIM0_CH2_LUT) bits
#define TIM_CH2_LUT_MASK (0xFFFF)    // Bits [15:0]: Channel 2 LUT
#define TIM_CH2_FLT_MASK (0x3 << 16) // Bits [17:16]: Channel 2 Filter

// -----------------------------------------------------------------------------
// Timer Channel 3 Threshold Register (REG_TIM0_CH3_TH) bits
// -----------------------------------------------------------------------------
#define TIM_CH3_TH_MASK (0xFFFF)      // Bits [15:0]: Channel 3 Threshold
#define TIM_CH3_MODE_MASK (0x7 << 16) // Bits [18:16]: Channel 3 Mode

// Timer Channel 3 LUT Register (REG_TIM0_CH3_LUT) bits
#define TIM_CH3_LUT_MASK (0xFFFF)    // Bits [15:0]: Channel 3 LUT
#define TIM_CH3_FLT_MASK (0x3 << 16) // Bits [17:16]: Channel 3 Filter

// -----------------------------------------------------------------------------
// Other Registers (no bitfield macros needed)
// -----------------------------------------------------------------------------
#define TIM_TH_MASK (0xFFFFFFFF) // Full 32 bits for Threshold (TH)

void timer0_init(uint32_t threshold);
uint32_t timer0_get_counter();
uint32_t timer0_get_threshold();

#endif // __TIMER_REGS_H__
