// Copyright 2022 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
//
// Nils Wistoff <nwistoff@iis.ee.ethz.ch>
// Paul Scheffler <paulsc@iis.ee.ethz.ch>
// Nico Canzani <ncanzani@student.ethz.ch>

#pragma once

// Address map
#define SOCCTRL_BASE_ADDR 0x03000000
#define UART_BASE_ADDR 0x03002000
#define GPIO_BASE_ADDR 0x03005000
#define TIMER_BASE_ADDR 0x0300A000
#define USER_ROM_BASE_ADDR 0x20000000
#define ADV_TIMER_BASE_ADDR 0x0300E000
#define PULSER_BASE_ADDR 0x0300C000

// Frequencies
#define TB_FREQUENCY 20000000
#define TB_BAUDRATE 115200

// Peripheral configs
// UART
#define UART_BYTE_ALIGN 4
#define UART_FREQ TB_FREQUENCY
#define UART_BAUD TB_BAUDRATE

// Since SRAM is very limmited, select which part to compile and test.
// Difficult to impossible to activate more than one test
#define TEST_READ_ROM               0
#define TEST_REG_PART_F1            0
#define TEST_REG_PART_F2            0
#define TEST_REG_PART_CNT           0
#define TEST_RUN_ALL_PULSERS        0
#define TEST_RUN_PULSER_ONE_BY_ONE  0
#define TEST_RUN_ADV_TIMER          1
