// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Author:
// - Nico Canzani <ncanzani@student.ethz.ch>

#include "util.h"
#include "config.h"
#include "user_pulser.h"

// TODO: Check read functions (start, stop might be wrong)

// Function to initialize the pulser
void pulser_set_values(pulser_id_t id, const pulser_settings_t *settings)
{
    pulser_set_f1_end_switch(id, settings->f1_end, settings->f1_switch);
    pulser_set_f2_end_switch(id, settings->f2_end, settings->f2_switch);
    pulser_set_f1_f2_stop_count(id, settings->f1_count, settings->f2_count, settings->stop_count);
}

// Function to configure pulser (output, pulsing numbers)
void pulser_config(pulser_id_t id, const pulser_settings_t *settings)
{
    pulser_set_values(id, settings);

    int val = (settings->invert_out & 0xF) |
              ((settings->idle_high << 4) & 0xF0);
    pulser_write(id, REG_PULSER_OUT_CTRL, val);
}

// Function to start the pulser
void pulser_start(int pulser_to_start)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER_CTRL_REG) = pulser_to_start & 0b1111;
}

// Function to stop the pulser
void pulser_stop(int pulser_to_stop)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER_CTRL_REG) = (pulser_to_stop & 0b1111) << 4;
}

// Setters for each of the pulser parameters
void pulser_set_f1_end_switch(pulser_id_t id, int endvalue, int switchvalue)
{
    int val = ((endvalue << PULSER_F1_END_OFFSET) & PULSER_F1_END_MASK) |
              ((switchvalue << PULSER_F1_SWITCH_OFFSET) & PULSER_F1_SWITCH_MASK);
    pulser_write(id, PULSER_F1_REG, val);
}

void pulser_set_f2_end_switch(pulser_id_t id, int endvalue, int switchvalue)
{
    int val = ((endvalue << PULSER_F2_END_OFFSET) & PULSER_F2_END_MASK) |
              ((switchvalue << PULSER_F2_SWITCH_OFFSET) & PULSER_F2_SWITCH_MASK);
    pulser_write(id, PULSER_F2_REG, val);
}

void pulser_set_f1_f2_stop_count(pulser_id_t id, int n_f1, int n_f2, int n_stop)
{
    int val = ((n_f1 << PULSER_N_F1_REG_OFFSET) & PULSER_COUNT_F1_MASK) |
              ((n_f2 << PULSER_N_F2_REG_OFFSET) & PULSER_COUNT_F2_MASK) |
              ((n_stop << PULSER_N_STOP_REG_OFFSET) & PULSER_COUNT_STOP_MASK);
    pulser_write(id, PULSER_COUNT_REG, val);
}

int pulser_read_start(pulser_id_t id)
{
    return pulser_read(id, PULSER_CTRL_REG) & PULSER_START_BIT;
}

int pulser_read_f1_end(pulser_id_t id)
{
    return (pulser_read(id, PULSER_F1_REG) & PULSER_F1_END_MASK) >> PULSER_F1_END_OFFSET;
}

int pulser_read_f1_switch(pulser_id_t id)
{
    return (pulser_read(id, PULSER_F1_REG) & PULSER_F1_SWITCH_MASK) >> PULSER_F1_SWITCH_OFFSET;
}

int pulser_read_f2_end(pulser_id_t id)
{
    return (pulser_read(id, PULSER_F2_REG) & PULSER_F2_END_MASK) >> PULSER_F2_END_OFFSET;
}

int pulser_read_f2_switch(pulser_id_t id)
{
    return (pulser_read(id, PULSER_F2_REG) & PULSER_F2_SWITCH_MASK) >> PULSER_F2_SWITCH_OFFSET;
}

int pulser_read_count(pulser_id_t id)
{
    return pulser_read(id, PULSER_COUNT_REG);
}

int pulser_read_status(pulser_id_t id)
{
    return pulser_read(id, PULSER_STATUS_REG) & (PULSER_READY_STATUS_BIT | PULSER_STATE_MASK);
}

int pulser_ready(pulser_id_t id)
{
    return pulser_read(id, PULSER_STATUS_REG) & PULSER_READY_STATUS_BIT;
}

state_pulser_t get_pulser_fsm_state(pulser_id_t id)
{
    int status = pulser_read(id, PULSER_STATUS_REG);
    return (state_pulser_t)((status & PULSER_STATE_MASK) >> 1);
}

static inline void pulser_write(pulser_id_t id, int reg_offset, int value)
{
    *reg32(USER_PULSER_BASE_ADDR + (id * PULSER_OFFSET_PER_ID), reg_offset) = value;
}

static inline int pulser_read(pulser_id_t id, int reg_offset)
{
    return *reg32(USER_PULSER_BASE_ADDR + (id * PULSER_OFFSET_PER_ID), reg_offset);
}
