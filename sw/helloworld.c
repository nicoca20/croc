// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Authors:
// - Philippe Sauter <phsauter@iis.ee.ethz.ch>
// - Nico Canzani <ncanzani@student.ethz.ch>

#include "uart.h"
#include "print.h"
#include "timer.h"
#include "gpio.h"
#include "util.h"
#include "adv_timer.h"
#include "user_pulser.h"

/// @brief Example integer square root
/// @return integer square root of n
uint32_t isqrt(uint32_t n) {
    uint32_t res = 0;
    uint32_t bit = (uint32_t)1 << 30;

    while (bit > n) bit >>= 2;

    while (bit) {
        if (n >= res + bit) {
            n -= res + bit;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return res;
}

char receive_buff[16] = {0};

int main() {
    uart_init(); // setup the uart peripheral

    // simple printf support (only prints text and hex numbers)
    printf("Hello World!\n");
    uart_write_flush();

    pulser_set_values();

    pulser_start();
    pulser_stop();
    for (int i = 0; i < 10; i++)
    {
        __asm__ volatile("nop");
    }
    pulser_start();

    // Check pulser reading functions
    printf("Pulser state fsm: 0x%x\n", get_pulser_fsm_state());
    uart_write_flush();
    printf("Reading f1 end: 0x%x\n", pulser_read_f1_end());
    uart_write_flush();
    printf("Reading f1 switch: 0x%x\n", pulser_read_f1_switch());
    uart_write_flush();
    printf("Reading f2 end: 0x%x\n", pulser_read_f2_end());
    uart_write_flush();
    printf("Reading f2 switch: 0x%x\n", pulser_read_f2_switch());
    uart_write_flush();
    printf("Reading count: 0x%x\n", pulser_read_count());
    uart_write_flush();
    printf("Reading status: 0x%x\n", pulser_read_status());
    uart_write_flush();
    printf("Pulser ready: 0x%x\n", pulser_ready());
    uart_write_flush();
    printf("Pulser state fsm: 0x%x\n", get_pulser_fsm_state());
    uart_write_flush();

    return 1;
}
