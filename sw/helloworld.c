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

    // uart loopback
    uart_loopback_enable();
    printf("internal msg\n");
    sleep_ms(1);
    for(uint8_t idx = 0; idx<15; idx++) {
        receive_buff[idx] = uart_read();
        if(receive_buff[idx] == '\n') {
            break;
        }
    }
    uart_loopback_disable();

    printf("Loopback received: ");
    printf(receive_buff);
    uart_write_flush();

    // toggling some GPIOs
    gpio_set_direction(0xFFFF, 0x000F); // lowest four as outputs
    gpio_write(0x0A);  // ready output pattern
    gpio_enable(0xFF); // enable lowest eight
    // wait a few cycles to give GPIO signal time to propagate
    asm volatile ("nop; nop; nop; nop; nop;");
    printf("GPIO (expect 0xA0): 0x%x\n", gpio_read());

    printf("Timer started\n");

    uart_write_flush();
    uint32_t counter_value = timer0_get_counter();
    printf("Timer Counter: 0x%x\n", counter_value);
    uart_write_flush();

    // for (int i = 0; i < 10; i++)
    // {
    //     uint32_t counter_value = timer0_get_counter();
    //     printf("Timer Counter [0x%x]: 0x%x\n", i, counter_value);
    //     uart_write_flush();
    // }

    // Wait for the timer to reach threshold
    // while (timer0_get_counter() < threshold)
    //     ;

    // printf("Timer reached threshold!\n");
    // uart_write_flush();
    return 1;
}
