// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Author:
// - Nico Canzani <ncanzani@student.ethz.ch>

#ifndef __PULSER_REGS_H__
#define __PULSER_REGS_H__

// Settings struct to store values to configure the pulser
typedef struct
{
    int f1_end;
    int f1_high;
    int f2_end;
    int f2_high;
    int f1_count;
    int f2_count;
    int stop_count;
} Pulser_Settings_t;

// Register offsets (based on the `addr_q` in Verilog)
#define PULSER_START_REG_OFFSET 0x00
#define PULSER_F1_REG_OFFSET 0x04
#define PULSER_F2_REG_OFFSET 0x08
#define PULSER_COUNT_REG_OFFSET 0x0C
#define PULSER_DONE_STATUS_REG 0x10

// Macros for bit field extractions (based on Verilog signal widths)
#define PULSER_START_BIT 0
#define PULSER_F1_HIGH_BITS 0xFFFF0000
#define PULSER_F1_END_BITS 0x0000FFFF
#define PULSER_F2_HIGH_BITS 0xFFFF0000
#define PULSER_F2_END_BITS 0x0000FFFF
#define PULSER_F1_COUNT_BITS 0x000000FF
#define PULSER_F2_COUNT_BITS 0x0000FF00
#define PULSER_STOP_COUNT_BITS 0x00FF0000
#define PULSER_DONE_STATUS_BIT 31

// Function prototypes
void pulser_set_values(Pulser_Settings_t *settings);
void pulser_start(void);
void pulser_stop(void);
void pulser_set_f1_end_high(int endvalue, int highvalue);
void pulser_set_f2_end_high(int endvalue, int highvalue);
void pulser_set_f1_f2_stop_count(int n_f1, int n_f2, int n_stop);
int pulser_read_start(void);
int pulser_read_f1_end(void);
int pulser_read_f1_high(void);
int pulser_read_f2_end(void);
int pulser_read_f2_high(void);
int pulser_read_count(void);
int pulser_read_done_status(void);

// Helper functions for memory-mapped register access
void pulser_write(int reg_offset, int value);
int pulser_read(int reg_offset);

#endif // __PULSER_REGS_H__
