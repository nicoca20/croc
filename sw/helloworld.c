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

    int topvalue = 0x10;
    int topvalue_read = timer0_get_top_value(); // Causes first APB communication
    printf("Timer Top Value before init: 0x%x\n", topvalue_read);
    timer0_init(topvalue);
    topvalue_read = timer0_get_top_value();
    printf("Timer Top Value after init: 0x%x (expexted 0x%x)\n", topvalue_read, topvalue);

    printf("Timer started\n");
    uart_write_flush();

    // Wait for the timer to reach some threshold
    int wait_until_counter = 0x200;
    while (timer0_get_counter() < wait_until_counter && wait_until_counter < topvalue)
        ;

    printf("Timer reached threshold of 0x%x!\n", wait_until_counter);
    uart_write_flush();

    return 1;
}
