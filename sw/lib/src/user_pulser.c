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
void pulser_set_values(Pulser_Settings_t *settings)
{
    // *reg32(USER_PULSER_BASE_ADDR, PULSER_F1_REG_OFFSET) = 1;
    pulser_set_f1_end_high(settings->f1_end, settings->f1_high);
    pulser_set_f2_end_high(settings->f2_end, settings->f2_high);
    pulser_set_f1_f2_stop_count(settings->f1_count, settings->f2_count, settings->stop_count);
}

// Function to start the pulser (set the start bit)
void pulser_start(void)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER_START_REG_OFFSET) = 1; // Set start bit to 1 using dereferencing
}

// Function to stop the pulser (clear the start bit)
void pulser_stop(void)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER_START_REG_OFFSET) |= (1 << 1);
}

// Setters for each of the pulser parameters
void pulser_set_f1_end_high(int endvalue, int highvalue)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER_F1_REG_OFFSET) = ((endvalue << 16) & PULSER_F1_HIGH_BITS) | (highvalue & PULSER_F1_END_BITS); // Masking with the correct bitfield
}

void pulser_set_f2_end_high(int endvalue, int highvalue)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER_F2_REG_OFFSET) = ((endvalue << 16) & PULSER_F2_HIGH_BITS) | (highvalue & PULSER_F2_END_BITS);
}

void pulser_set_f1_f2_stop_count(int n_f1, int n_f2, int n_stop)
{
    *reg32(USER_PULSER_BASE_ADDR, PULSER_COUNT_REG_OFFSET) = (n_f1 << 0) | (n_f2 << 8) | (n_stop << 16);
}

// Read functions to fetch the current values of the pulser registers
int pulser_read_start(void)
{
    return *reg32(USER_PULSER_BASE_ADDR, PULSER_START_REG_OFFSET) & PULSER_START_BIT;
}

int pulser_read_f1_end(void)
{
    return *reg32(USER_PULSER_BASE_ADDR, PULSER_F1_REG_OFFSET) & PULSER_F1_END_BITS;
}

int pulser_read_f1_high(void)
{
    return *reg32(USER_PULSER_BASE_ADDR, PULSER_F1_REG_OFFSET) & PULSER_F1_HIGH_BITS;
}

int pulser_read_f2_end(void)
{
    return *reg32(USER_PULSER_BASE_ADDR, PULSER_F2_REG_OFFSET) & PULSER_F2_END_BITS;
}

int pulser_read_f2_high(void)
{
    return *reg32(USER_PULSER_BASE_ADDR, PULSER_F2_REG_OFFSET) & PULSER_F2_HIGH_BITS;
}

int pulser_read_count(void)
{
    return *reg32(USER_PULSER_BASE_ADDR, PULSER_COUNT_REG_OFFSET);
}

int pulser_read_done_status(void)
{
    return *reg32(USER_PULSER_BASE_ADDR, PULSER_DONE_STATUS_REG) & PULSER_DONE_STATUS_BIT;
}

void pulser_write(int reg_offset, int value)
{
    *reg32(USER_PULSER_BASE_ADDR, reg_offset) = value;
}

int pulser_read(int reg_offset)
{
    return *reg32(USER_PULSER_BASE_ADDR, reg_offset);
}
