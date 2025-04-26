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
    int f1_switch;
    int f2_end;
    int f2_switch;
    int f1_count;
    int f2_count;
    int stop_count;
} pulser_settings_t;

// Finite state Machine states of pulser
typedef enum
{
    IDLE = 0,
    RUN_F1 = 1,
    RUN_F2 = 2,
    RUN_STOP = 3,
    DONE = 4
} state_pulser_t;

typedef enum
{
    PULSER_0 = 0,
    PULSER_1 = 1,
    PULSER_2 = 2,
    PULSER_3 = 3
} pulser_id_t;

#define PULSER_OFFSET_PER_ID 0x20 // space for each pulser

// Register offsets (based on the `addr_q` in Verilog)
#define PULSER_CTRL_REG 0x00
#define PULSER_F1_REG 0x04
#define PULSER_F2_REG 0x08
#define PULSER_COUNT_REG 0x0C
#define PULSER_STATUS_REG 0x10

// Macros for bit field extractions (based on Verilog signal widths)
#define PULSER_START_BIT (1 << 0)
#define PULSER_STOP_BIT (1 << 1)
#define PULSER_READY_STATUS_BIT (1 << 0)
#define PULSER_STATE_MASK (0x7 << 1)

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
void pulser_set_values(pulser_id_t id, const pulser_settings_t *settings);
void pulser_start(int pulser_to_start);
void pulser_stop(int pulser_to_stop);

void pulser_set_f1_end_switch(pulser_id_t id, int endvalue, int switchvalue);
void pulser_set_f2_end_switch(pulser_id_t id, int endvalue, int switchvalue);
void pulser_set_f1_f2_stop_count(pulser_id_t id, int n_f1, int n_f2, int n_stop);

int pulser_read_start(pulser_id_t id);
int pulser_read_f1_end(pulser_id_t id);
int pulser_read_f1_switch(pulser_id_t id);
int pulser_read_f2_end(pulser_id_t id);
int pulser_read_f2_switch(pulser_id_t id);
int pulser_read_count(pulser_id_t id);
int pulser_read_status(pulser_id_t id);
int pulser_ready(pulser_id_t id);
state_pulser_t get_pulser_fsm_state(pulser_id_t id);

// Low-level memory access
static inline void pulser_write(pulser_id_t id, int reg_offset, int value);
static inline int pulser_read(pulser_id_t id, int reg_offset);

#endif // __PULSER_REGS_H__
