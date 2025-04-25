// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Author:
// - Nico Canzani <ncanzani@student.ethz.ch>

#include "util.h"
#include "config.h"
#include "user_pulser.h"

// Function to initialize the pulser (can be expanded for actual hardware initialization)
void pulser_set_values(const pulser_settings_t *settings)
{
    // *reg32(USER_PULSER_BASE_ADDR, PULSER_F1_REG_OFFSET) = 1;
    pulser_set_f1_end_switch(settings->f1_end, settings->f1_high);
    pulser_set_f2_end_switch(settings->f2_end, settings->f2_high);
    pulser_set_f1_f2_stop_count(settings->f1_count, settings->f2_count, settings->stop_count);
}

// Function to start the pulser (set the start bit)
void pulser_start(void)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER0_CFG_REG) |= PULSER_START_BIT;
}

// Function to stop the pulser (clear the start bit)
void pulser_stop(void)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER0_CFG_REG) |= PULSER_STOP_BIT;
}

// Setters for each of the pulser parameters
void pulser_set_f1_end_switch(int endvalue, int switchvalue)
{
    int val = ((endvalue << PULSER_F1_END_OFFSET) & PULSER_F1_END_MASK) |
              ((switchvalue << PULSER_F1_SWITCH_OFFSET) & PULSER_F1_SWITCH_MASK);

    pulser_write(PULSER0_F1_REG, val);
}

void pulser_set_f2_end_switch(int endvalue, int switchvalue)
{
    int val = ((endvalue << PULSER_F2_END_OFFSET) & PULSER_F2_END_MASK) |
              ((switchvalue << PULSER_F2_SWITCH_OFFSET) & PULSER_F2_SWITCH_MASK);

    pulser_write(PULSER0_F2_REG, val);
}

void pulser_set_f1_f2_stop_count(int n_f1, int n_f2, int n_stop)
{
    int val = ((n_f1 << PULSER_N_F1_REG_OFFSET) & PULSER_COUNT_F1_MASK) |
              ((n_f2 << PULSER_N_F2_REG_OFFSET) & PULSER_COUNT_F2_MASK) |
              ((n_stop << PULSER_N_STOP_REG_OFFSET) & PULSER_COUNT_STOP_MASK);

    pulser_write(PULSER0_COUNT_REG, val);
}

// Read functions to fetch the current values of the pulser registers
int pulser_read_start(void)
{
    return pulser_read(PULSER0_CFG_REG) & PULSER_START_BIT;
}

int pulser_read_f1_end(void)
{
    return (pulser_read(PULSER0_F1_REG) & PULSER_F1_END_MASK) >> PULSER_F1_END_OFFSET;
}

int pulser_read_f1_switch(void)
{
    return (pulser_read(PULSER0_F1_REG) & PULSER_F1_SWITCH_MASK) >> PULSER_F1_SWITCH_OFFSET;
}

int pulser_read_f2_end(void)
{
    return (pulser_read(PULSER0_F2_REG) & PULSER_F2_END_MASK) >> PULSER_F2_END_OFFSET;
}

int pulser_read_f2_switch(void)
{
    return (pulser_read(PULSER0_F2_REG) & PULSER_F2_SWITCH_MASK) >> PULSER_F2_SWITCH_OFFSET;
}

int pulser_read_count(void)
{
    return pulser_read(PULSER0_COUNT_REG);
}

int pulser_read_status(void)
{
    return pulser_read(PULSER0_STATUS_REG) & (PULSER_READY_STATUS_BIT | PULSER_STATE_MASK);
}

int pulser_ready(void)
{
    return pulser_read(PULSER0_STATUS_REG) & PULSER_READY_STATUS_BIT;
}

state_pulser_t get_pulser_fsm_state(void)
{
    state_pulser_t current_state_pulser = (pulser_read_status() & PULSER_STATE_MASK) >> 1;
    return current_state_pulser;
}

void pulser_write(int reg_offset, int value)
{
    *reg32(USER_PULSER_BASE_ADDR, reg_offset) = value;
}

int pulser_read(int reg_offset)
{
    return *reg32(USER_PULSER_BASE_ADDR, reg_offset);
}
