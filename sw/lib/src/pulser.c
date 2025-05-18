// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Author:
// - Nico Canzani <ncanzani@student.ethz.ch>

#include "util.h"
#include "config.h"
#include "pulser.h"
// Low-level register access helpers
static inline void pulser_write(pulser_id_t id, int reg_offset, int value)
{
    *reg32(PULSER_BASE_ADDR + (id * PULSER_OFFSET_PER_ID), reg_offset) = value;
}

static inline int pulser_read(pulser_id_t id, int reg_offset)
{
    return *reg32(PULSER_BASE_ADDR + (id * PULSER_OFFSET_PER_ID), reg_offset);
}

uint32_t bitfield_get_field32(bitfield_field32_t field, uint32_t reg)
{
    return (reg >> field.index) & field.mask;
}

uint32_t bitfield_set_field32(bitfield_field32_t field, uint32_t reg, uint32_t value)
{
    reg &= ~(field.mask << field.index);
    reg |= ((value & field.mask) << field.index);
    return reg;
}

// Set phase F1 end and switch values
void pulser_set_f1_end_switch(pulser_id_t id, int endvalue, int switchvalue)
{
    int reg = 0;
    reg = bitfield_set_field32(PULSER_F1_CFG_SWITCH_FIELD, reg, (uint32_t)switchvalue);
    reg = bitfield_set_field32(PULSER_F1_CFG_ENDVAL_FIELD, reg, (uint32_t)endvalue);
    pulser_write(id, PULSER_F1_CFG_REG_OFFSET, reg);
}

// Set phase F2 end and switch values
void pulser_set_f2_end_switch(pulser_id_t id, int endvalue, int switchvalue)
{
    int reg = 0;
    reg = bitfield_set_field32(PULSER_F2_CFG_SWITCH_FIELD, reg, (uint32_t)switchvalue);
    reg = bitfield_set_field32(PULSER_F2_CFG_ENDVAL_FIELD, reg, (uint32_t)endvalue);
    pulser_write(id, PULSER_F2_CFG_REG_OFFSET, reg);
}

// Set pulse counts for F1, F2 and stop
void pulser_set_f1_f2_stop_count(pulser_id_t id, int n_f1, int n_f2, int n_stop)
{
    int reg = 0;
    reg = bitfield_set_field32(PULSER_COUNT_CFG_F1_FIELD, reg, (uint32_t)n_f1);
    reg = bitfield_set_field32(PULSER_COUNT_CFG_F2_FIELD, reg, (uint32_t)n_f2);
    reg = bitfield_set_field32(PULSER_COUNT_CFG_COUNT_STOP_FIELD, reg, (uint32_t)n_stop);
    pulser_write(id, PULSER_COUNT_CFG_REG_OFFSET, reg);
}

// Configure all pulser settings at once
void pulser_set_values(pulser_id_t id, const pulser_settings_t *settings)
{
    pulser_set_f1_end_switch(id, settings->f1_end, settings->f1_switch);
    pulser_set_f2_end_switch(id, settings->f2_end, settings->f2_switch);
    pulser_set_f1_f2_stop_count(id, settings->f1_count, settings->f2_count, settings->stop_count);
}

// Configure pulser with output control settings
void pulser_config(pulser_id_t id, const pulser_settings_t *settings)
{
    pulser_set_values(id, settings);
    int reg = 0;
    if (settings->invert_out)
    {
        reg |= (1 << PULSER_OUT_CTRL_INVERT_OUT_BIT);
    }
    if (settings->idle_high)
    {
        reg |= (1 << PULSER_OUT_CTRL_IDLE_OUT_BIT);
    }
    pulser_write(id, PULSER_OUT_CTRL_REG_OFFSET, reg);
}

// Start the pulser by writing to control register
void pulser_start(int pulser_to_start)
{
    // Assuming start bits are bit 0..3 in CTRL reg
    // No dedicated control reg given in new reg map, simulate with base addr + 0x14 for example
    volatile uint32_t *ctrl_reg = (volatile uint32_t *)(PULSER_BASE_ADDR + 0x14);
    *ctrl_reg = (uint32_t)(pulser_to_start & 0xF);
}

// Stop the pulser by writing to control register
void pulser_stop(int pulser_to_stop)
{
    // Assuming stop bits shifted by 4
    volatile uint32_t *ctrl_reg = (volatile uint32_t *)(PULSER_BASE_ADDR + 0x14);
    *ctrl_reg = ((uint32_t)(pulser_to_stop & 0xF)) << 4;
}

// Reading functions
int pulser_read_start(pulser_id_t id)
{
    volatile uint32_t *ctrl_reg = (volatile uint32_t *)(PULSER_BASE_ADDR + 0x14);
    (void)id;
    return (*ctrl_reg) & 0x1; // Assuming start bit is bit0
}

int pulser_read_f1_end(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_F1_CFG_REG_OFFSET);
    return (int)bitfield_get_field32(PULSER_F1_CFG_ENDVAL_FIELD, (uint32_t)reg);
}

int pulser_read_f1_switch(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_F1_CFG_REG_OFFSET);
    return (int)bitfield_get_field32(PULSER_F1_CFG_SWITCH_FIELD, (uint32_t)reg);
}

int pulser_read_f2_end(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_F2_CFG_REG_OFFSET);
    return (int)bitfield_get_field32(PULSER_F2_CFG_ENDVAL_FIELD, (uint32_t)reg);
}

int pulser_read_f2_switch(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_F2_CFG_REG_OFFSET);
    return (int)bitfield_get_field32(PULSER_F2_CFG_SWITCH_FIELD, (uint32_t)reg);
}

int pulser_read_count(pulser_id_t id)
{
    return pulser_read(id, PULSER_COUNT_CFG_REG_OFFSET);
}

int pulser_read_status(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_STATUS_REG_OFFSET);
    int ready_bit = (reg >> PULSER_STATUS_READY_BIT) & 0x1;
    int state = (reg >> PULSER_STATUS_STATE_OFFSET) & PULSER_STATUS_STATE_MASK;
    return (ready_bit | (state << 1));
}

int pulser_ready(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_STATUS_REG_OFFSET);
    return (reg >> PULSER_STATUS_READY_BIT) & 0x1;
}

state_pulser_t get_pulser_fsm_state(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_STATUS_REG_OFFSET);
    int state = (reg >> PULSER_STATUS_STATE_OFFSET) & PULSER_STATUS_STATE_MASK;
    return (state_pulser_t)state;
}