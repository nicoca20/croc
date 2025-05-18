// Generated register defines for pulser

// Copyright information found in source file:
// Copyright lowRISC contributors (OpenTitan project).

// Licensing information found in source file:
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef _PULSER_REG_DEFS_
#define _PULSER_REG_DEFS_

#ifdef __cplusplus
extern "C" {
#endif
// Register width
#define PULSER_PARAM_REG_WIDTH 32

// Phase F1 configuration: end and switch points
#define PULSER_F1_CFG_REG_OFFSET 0x0
#define PULSER_F1_CFG_SWITCH_MASK 0xffff
#define PULSER_F1_CFG_SWITCH_OFFSET 0
#define PULSER_F1_CFG_SWITCH_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_F1_CFG_SWITCH_MASK, .index = PULSER_F1_CFG_SWITCH_OFFSET })
#define PULSER_F1_CFG_ENDVAL_MASK 0xffff
#define PULSER_F1_CFG_ENDVAL_OFFSET 16
#define PULSER_F1_CFG_ENDVAL_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_F1_CFG_ENDVAL_MASK, .index = PULSER_F1_CFG_ENDVAL_OFFSET })

// Phase F2 configuration: end and switch points
#define PULSER_F2_CFG_REG_OFFSET 0x4
#define PULSER_F2_CFG_SWITCH_MASK 0xffff
#define PULSER_F2_CFG_SWITCH_OFFSET 0
#define PULSER_F2_CFG_SWITCH_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_F2_CFG_SWITCH_MASK, .index = PULSER_F2_CFG_SWITCH_OFFSET })
#define PULSER_F2_CFG_ENDVAL_MASK 0xffff
#define PULSER_F2_CFG_ENDVAL_OFFSET 16
#define PULSER_F2_CFG_ENDVAL_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_F2_CFG_ENDVAL_MASK, .index = PULSER_F2_CFG_ENDVAL_OFFSET })

// Configure number of pulses for each phase
#define PULSER_COUNT_CFG_REG_OFFSET 0x8
#define PULSER_COUNT_CFG_F1_MASK 0xff
#define PULSER_COUNT_CFG_F1_OFFSET 0
#define PULSER_COUNT_CFG_F1_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_COUNT_CFG_F1_MASK, .index = PULSER_COUNT_CFG_F1_OFFSET })
#define PULSER_COUNT_CFG_F2_MASK 0xff
#define PULSER_COUNT_CFG_F2_OFFSET 8
#define PULSER_COUNT_CFG_F2_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_COUNT_CFG_F2_MASK, .index = PULSER_COUNT_CFG_F2_OFFSET })
#define PULSER_COUNT_CFG_COUNT_STOP_MASK 0xff
#define PULSER_COUNT_CFG_COUNT_STOP_OFFSET 16
#define PULSER_COUNT_CFG_COUNT_STOP_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_COUNT_CFG_COUNT_STOP_MASK, .index = PULSER_COUNT_CFG_COUNT_STOP_OFFSET })

// Read state of pulser
#define PULSER_STATUS_REG_OFFSET 0xc
#define PULSER_STATUS_READY_BIT 0
#define PULSER_STATUS_STATE_MASK 0x7
#define PULSER_STATUS_STATE_OFFSET 1
#define PULSER_STATUS_STATE_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_STATUS_STATE_MASK, .index = PULSER_STATUS_STATE_OFFSET })

// Set idle state of pulser and if pulse phase should be inverted
#define PULSER_OUT_CTRL_REG_OFFSET 0x10
#define PULSER_OUT_CTRL_INVERT_OUT_BIT 0
#define PULSER_OUT_CTRL_IDLE_OUT_BIT 1

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // _PULSER_REG_DEFS_
// End generated register defines for pulser