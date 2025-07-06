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
    *reg32(PULSER_BASE_ADDR , reg_offset + id * PULSER_OFFSET_PER_ID) = value;
}

static inline int pulser_read(pulser_id_t id, int reg_offset)
{
    return *reg32(PULSER_BASE_ADDR, reg_offset + id * PULSER_OFFSET_PER_ID);
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
    reg = bitfield_set_field32(PULSER_CORE_CFG_F1_SWITCHVAL_FIELD, reg, (uint32_t)switchvalue);
    reg = bitfield_set_field32(PULSER_CORE_CFG_F1_ENDVAL_FIELD, reg, (uint32_t)endvalue);
    pulser_write(id, PULSER_CORE_CFG_F1_REG_OFFSET, reg);
}

// Set phase F2 end and switch values
void pulser_set_f2_end_switch(pulser_id_t id, int endvalue, int switchvalue)
{
    int reg = 0;
    reg = bitfield_set_field32(PULSER_CORE_CFG_F2_SWITCHVAL_FIELD, reg, (uint32_t)switchvalue);
    reg = bitfield_set_field32(PULSER_CORE_CFG_F2_ENDVAL_FIELD, reg, (uint32_t)endvalue);
    pulser_write(id, PULSER_CORE_CFG_F2_REG_OFFSET, reg);
}

// Set pulse counts for F1, F2 and stop
void pulser_set_f1_f2_stop_count(pulser_id_t id, int n_f1, int n_f2, int n_stop)
{
    int reg = 0;
    reg = bitfield_set_field32(PULSER_CORE_CFG_CNT_F1_FIELD, reg, (uint32_t)n_f1);
    reg = bitfield_set_field32(PULSER_CORE_CFG_CNT_F2_FIELD, reg, (uint32_t)n_f2);
    reg = bitfield_set_field32(PULSER_CORE_CFG_CNT_CNT_STOP_FIELD, reg, (uint32_t)n_stop);
    pulser_write(id, PULSER_CORE_CFG_CNT_REG_OFFSET, reg);
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
        reg |= (1 << PULSER_CORE_CTRL_OUT_INVERT_OUT_BIT);
    }
    if (settings->idle_high)
    {
        reg |= (1 << PULSER_CORE_CTRL_OUT_IDLE_OUT_BIT);
    }
    pulser_write(id, PULSER_CORE_CTRL_OUT_REG_OFFSET, reg);
}

// Enable the pulser by writing to config register
void pulser_en(int pulser_to_en)
{
    uint32_t offset_pulsers = PULSER_OFFSET_PER_ID * N_PULSERS;
    volatile uint32_t *cfg_reg = (volatile uint32_t *)(PULSER_BASE_ADDR + offset_pulsers + PULSER_GENERAL_CFG_REG_OFFSET);

    uint32_t regval = *cfg_reg;
    *cfg_reg = regval | (pulser_to_en << PULSER_GENERAL_CFG_EN_OFFSET);
}

// Disable the pulser by writing to config register
void pulser_dis(int pulser_to_dis)
{
    uint32_t offset_pulsers = PULSER_OFFSET_PER_ID * N_PULSERS;
    volatile uint32_t *cfg_reg = (volatile uint32_t *)(PULSER_BASE_ADDR + offset_pulsers + PULSER_GENERAL_CFG_REG_OFFSET);

    uint32_t regval = *cfg_reg;
    *cfg_reg = regval & ~(pulser_to_dis << PULSER_GENERAL_CFG_EN_OFFSET);
}

// Start the pulser by writing to control register
void pulser_start(int pulser_to_start)
{
    uint32_t offset_pulsers = PULSER_OFFSET_PER_ID * N_PULSERS;
    volatile uint32_t *ctrl_reg = (volatile uint32_t *)(PULSER_BASE_ADDR + offset_pulsers + PULSER_GENERAL_CTRL_REG_OFFSET);
    *ctrl_reg = (uint32_t)(pulser_to_start & PULSER_GENERAL_CTRL_START_MASK) << PULSER_GENERAL_CTRL_START_OFFSET;
}

// Stop the pulser by writing to control register
void pulser_stop(int pulser_to_stop)
{
    uint32_t offset_pulsers = PULSER_OFFSET_PER_ID * N_PULSERS;
    volatile uint32_t *ctrl_reg = (volatile uint32_t *)(PULSER_BASE_ADDR + offset_pulsers + PULSER_GENERAL_CTRL_REG_OFFSET);
    *ctrl_reg = ((uint32_t)(pulser_to_stop & PULSER_GENERAL_CTRL_STOP_MASK)) << PULSER_GENERAL_CTRL_STOP_OFFSET;
}

// void pulser_disable_all_after_done(void)
// {
//     for (int i_pulser = 0; i_pulser < N_PULSERS; ++i_pulser) {
//         while (!pulser_ready(i_pulser)) ;
//         pulser_dis(1 << i_pulser);
//     }
// }

// Reading functions

int pulser_read_f1_end(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_CORE_CFG_F1_REG_OFFSET);
    return (int)bitfield_get_field32(PULSER_CORE_CFG_F1_ENDVAL_FIELD, (uint32_t)reg);
}

int pulser_read_f1_switch(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_CORE_CFG_F1_REG_OFFSET);
    return (int)bitfield_get_field32(PULSER_CORE_CFG_F1_SWITCHVAL_FIELD, (uint32_t)reg);
}

int pulser_read_f2_end(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_CORE_CFG_F2_REG_OFFSET);
    return (int)bitfield_get_field32(PULSER_CORE_CFG_F2_ENDVAL_FIELD, (uint32_t)reg);
}

int pulser_read_f2_switch(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_CORE_CFG_F2_REG_OFFSET);
    return (int)bitfield_get_field32(PULSER_CORE_CFG_F2_SWITCHVAL_FIELD, (uint32_t)reg);
}

int pulser_read_count(pulser_id_t id)
{
    return pulser_read(id, PULSER_CORE_CFG_CNT_REG_OFFSET);
}

// int pulser_read_status(pulser_id_t id)
// {
//     int reg = pulser_read(id, PULSER_CORE_STATUS_REG_OFFSET);
//     int ready_bit = (reg & (1 << PULSER_CORE_STATUS_READY_BIT)) >> PULSER_CORE_STATUS_READY_BIT;
//     int state = (reg >> PULSER_CORE_STATUS_STATE_OFFSET) & PULSER_CORE_STATUS_STATE_MASK;
//     return (ready_bit | (state << 1));
// }

// int pulser_ready(pulser_id_t id)
// {
//     int reg = pulser_read(id, PULSER_CORE_STATUS_REG_OFFSET);
//     return (reg & (1 << PULSER_CORE_STATUS_READY_BIT)) >> PULSER_CORE_STATUS_READY_BIT;
// }

state_pulser_t get_pulser_fsm_state(pulser_id_t id)
{
    int reg = pulser_read(id, PULSER_CORE_STATUS_REG_OFFSET);
    int state = bitfield_get_field32(PULSER_CORE_STATUS_STATE_FIELD, reg);
    return (state_pulser_t)state;
}
