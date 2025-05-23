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
#include "pulser.h"

#define PULSER_TO_TEST PULSER_0

int main()
{

    uart_init(); // setup the uart peripheral

    // simple printf support (only prints text and hex numbers)
    printf("Hello World!\n");
    uart_write_flush();

    pulser_settings_t pulser0_settings;
    pulser0_settings.f1_end = 7;
    pulser0_settings.f1_switch = 3;
    pulser0_settings.f2_end = 9;
    pulser0_settings.f2_switch = 6;
    pulser0_settings.f1_count = 8;
    pulser0_settings.f2_count = 5;
    pulser0_settings.stop_count = 2;
    pulser0_settings.invert_out = 0;
    pulser0_settings.idle_high = 0;

    pulser_settings_t pulser1_settings;
    pulser1_settings.f1_end = 8;
    pulser1_settings.f1_switch = 4;
    pulser1_settings.f2_end = 6;
    pulser1_settings.f2_switch = 2;
    pulser1_settings.f1_count = 1;
    pulser1_settings.f2_count = 9;
    pulser1_settings.stop_count = 3;
    pulser1_settings.invert_out = 0;
    pulser1_settings.idle_high = 0;

    pulser_settings_t pulser2_settings;
    pulser2_settings.f1_end = 9;
    pulser2_settings.f1_switch = 5;
    pulser2_settings.f2_end = 7;
    pulser2_settings.f2_switch = 3;
    pulser2_settings.f1_count = 2;
    pulser2_settings.f2_count = 4;
    pulser2_settings.stop_count = 6;
    pulser2_settings.invert_out = 0;
    pulser2_settings.idle_high = 0;

    pulser_settings_t pulser3_settings;
    pulser3_settings.f1_end = 6;
    pulser3_settings.f1_switch = 2;
    pulser3_settings.f2_end = 8;
    pulser3_settings.f2_switch = 5;
    pulser3_settings.f1_count = 7;
    pulser3_settings.f2_count = 1;
    pulser3_settings.stop_count = 9;
    pulser3_settings.invert_out = 0;
    pulser3_settings.idle_high = 0;

    pulser_config(PULSER_0, &pulser0_settings);
    pulser_config(PULSER_1, &pulser1_settings);
    pulser_config(PULSER_2, &pulser2_settings);
    pulser_config(PULSER_3, &pulser3_settings);

    // Test Pulser 0

    pulser_start(1 << PULSER_0);
    pulser_stop(1 << PULSER_0);
    for (int i = 0; i < 2; i++)
    {
        __asm__ volatile("nop");
    }
    pulser_start(1 << PULSER_0);

    // Test Pulser 1

    // pulser_start(1 << PULSER_1);
    // pulser_stop(1 << PULSER_1);
    // for (int i = 0; i < 2; i++)
    // {
    //     __asm__ volatile("nop");
    // }
    // pulser_start(1 << PULSER_1);

    // // Test Pulser 2

    // pulser_start(1 << PULSER_2);
    // pulser_stop(1 << PULSER_2);
    // for (int i = 0; i < 2; i++)
    // {
    //     __asm__ volatile("nop");
    // }
    // pulser_start(1 << PULSER_2);

    // // Test Pulser 3

    // pulser_start(1 << PULSER_3);
    // pulser_stop(1 << PULSER_3);
    // for (int i = 0; i < 2; i++)
    // {
    //     __asm__ volatile("nop");
    // }
    // pulser_start(1 << PULSER_3);

    // // Test All pulsers at same time
    // for (int i = 0; i < 50; i++)
    // {
    //     __asm__ volatile("nop");
    // }

    pulser_start((1 << PULSER_0) | (1 << PULSER_1) | (1 << PULSER_2) | (1 << PULSER_3));
    pulser_stop((1 << PULSER_0) | (1 << PULSER_1) | (1 << PULSER_2) | (1 << PULSER_3));
    for (int i = 0; i < 2; i++)
    {
        __asm__ volatile("nop");
    }
    pulser_start((1 << PULSER_0) | (1 << PULSER_1) | (1 << PULSER_2) | (1 << PULSER_3));

    // // Check pulser reading functions
    // // printf("Pulser state fsm: 0x%x\n", get_pulser_fsm_state(PULSER_TO_TEST));
    // // uart_write_flush();
    // // printf("Reading f1 end: 0x%x\n", pulser_read_f1_end(PULSER_TO_TEST));
    // // uart_write_flush();
    // // printf("Reading f1 switch: 0x%x\n", pulser_read_f1_switch(PULSER_TO_TEST));
    // // uart_write_flush();
    // // printf("Reading f2 end: 0x%x\n", pulser_read_f2_end(PULSER_TO_TEST));
    // // uart_write_flush();
    // // printf("Reading f2 switch: 0x%x\n", pulser_read_f2_switch(PULSER_TO_TEST));
    // // uart_write_flush();
    // // printf("Reading count: 0x%x\n", pulser_read_count(PULSER_TO_TEST));
    // // uart_write_flush();
    // // printf("Reading status: 0x%x\n", pulser_read_status(PULSER_TO_TEST));
    // // uart_write_flush();
    // // printf("Pulser ready: 0x%x\n", pulser_ready(PULSER_TO_TEST));
    // // uart_write_flush();
    // // printf("Pulser state fsm: 0x%x\n", get_pulser_fsm_state(PULSER_TO_TEST));
    // // uart_write_flush();

    // Read Signature from ROM
    for (int i = 0; i < 3; i++)
    {
        printf("Rom data: %x\n", *reg32(USER_ROM_BASE_ADDR, i * 4));
        uart_write_flush();
    }

    return 1;
}
