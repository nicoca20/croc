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
    // wait until uart has finished sending
    uart_write_flush();

    Pulser_Settings_t firstPulserSettings = {
        .f1_end = 4,
        .f1_high = 1,
        .f2_end = 4,
        .f2_high = 2,
        .f1_count = 3,
        .f2_count = 0,
        .stop_count = 2};

    pulser_set_values(&firstPulserSettings);

    pulser_start();
    pulser_stop();
    for (int i = 0; i < 10; i++)
    {
        __asm__ volatile("nop");
    }
    pulser_start();
    // while (!pulser_read_done_status())
    // {
    //     // Busy-wait until the pulser has finished its task
    // }

    printf("Done with pulse sequence\n");
    uart_write_flush();

    return 1;
}
