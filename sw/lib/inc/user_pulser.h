// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Author:
// - Nico Canzani <ncanzani@student.ethz.ch>

#ifndef __PULSER_REGS_H__
#define __PULSER_REGS_H__

// Register offsets (based on the `addr_q` in Verilog)
#define PULSER0_CFG_REG 0x00
#define PULSER0_F1_REG 0x04
#define PULSER0_F2_REG 0x08
#define PULSER0_COUNT_REG 0x0C
#define PULSER0_STATUS_REG 0x10

// Macros for bit field extractions (based on Verilog signal widths)
#define PULSER_START_BIT (1 << 0)
#define PULSER_STOP_BIT (1 << 1)
#define PULSER_READY_STATUS_BIT (1 << 0)

#define PULSER_F1_SWITCH_MASK 0x0000FFFF
#define PULSER_F1_END_MASK 0xFFFF0000
#define PULSER_F2_SWITCH_MASK 0x0000FFFF
#define PULSER_F2_END_MASK 0xFFFF0000
#define PULSER_COUNT_F1_MASK 0x000000FF
#define PULSER_COUNT_F2_MASK 0x0000FF00
#define PULSER_COUNT_STOP_MASK 0x00FF0000

#define PULSER_F1_SWITCH_OFFSET 0
#define PULSER_F1_END_OFFSET 16
#define PULSER_F2_SWITCH_OFFSET 0
#define PULSER_F2_END_OFFSET 16
#define PULSER_STOP_SWITCH_OFFSET 0
#define PULSER_STOP_END_OFFSET 16

#define PULSER_N_F1_REG_OFFSET 0
#define PULSER_N_F2_REG_OFFSET 8
#define PULSER_N_STOP_REG_OFFSET 16

// Function prototypes
void pulser_set_values();
void pulser_start(void);
void pulser_stop(void);
void pulser_set_f1_end_switch(int endvalue, int switchvalue);
void pulser_set_f2_end_switch(int endvalue, int switchvalue);
void pulser_set_f1_f2_stop_count(int n_f1, int n_f2, int n_stop);
int pulser_read_start(void);
int pulser_read_f1_end(void);
int pulser_read_f1_switch(void);
int pulser_read_f2_end(void);
int pulser_read_f2_switch(void);
int pulser_read_count(void);
int pulser_read_done_status(void);

// Helper functions for memory-mapped register access
void pulser_write_int(int reg_offset, int value);
int pulser_read_int(int reg_offset);

#endif // __PULSER_REGS_H__
