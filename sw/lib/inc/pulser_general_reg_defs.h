// Generated register defines for pulser_general

// Copyright information found in source file:
// Copyright lowRISC contributors (OpenTitan project).

// Licensing information found in source file:
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef _PULSER_GENERAL_REG_DEFS_
#define _PULSER_GENERAL_REG_DEFS_

#ifdef __cplusplus
extern "C" {
#endif
// Register width
#define PULSER_GENERAL_PARAM_REG_WIDTH 32

// Start and Stop the pulser. No registers generated, commands handled in
// pulser.
#define PULSER_GENERAL_CTRL_REG_OFFSET 0x0
#define PULSER_GENERAL_CTRL_START_MASK 0xffff
#define PULSER_GENERAL_CTRL_START_OFFSET 0
#define PULSER_GENERAL_CTRL_START_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_GENERAL_CTRL_START_MASK, .index = PULSER_GENERAL_CTRL_START_OFFSET })
#define PULSER_GENERAL_CTRL_STOP_MASK 0xffff
#define PULSER_GENERAL_CTRL_STOP_OFFSET 16
#define PULSER_GENERAL_CTRL_STOP_FIELD \
  ((bitfield_field32_t) { .mask = PULSER_GENERAL_CTRL_STOP_MASK, .index = PULSER_GENERAL_CTRL_STOP_OFFSET })

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // _PULSER_GENERAL_REG_DEFS_
// End generated register defines for pulser_general