// Copyright (c) 2024 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0/
//
// Author:
// - Nico Canzani <ncanzani@student.ethz.ch>

#include "config.h"
#include "test_own_rtl.h"
#include "util.h"
#include "uart.h"
#include "print.h"


#if TEST_NOP
void test_nop (void) {
    // This is a test that does nothing, just to check if the test framework works

    // Use read to find start and stop of nop test
    *reg32(USER_ROM_BASE_ADDR, 0);
    for (int i = 0; i < 1000; i++)
    {
        __asm__ volatile("nop");
    }
    *reg32(USER_ROM_BASE_ADDR, 0);
}
#endif

#if TEST_READ_ROM
void test_read_rom(void) {
    // Read Signature from ROM
    for (int i = 0; i < 3; i++)
    {
        printf("Rom data: %x\n", *reg32(USER_ROM_BASE_ADDR, i * 4));
        uart_write_flush();
    }
}
#endif

#if TEST_REG_PART_F1 || TEST_REG_PART_F2 || TEST_REG_PART_CNT
void test_pulser_regs(pulser_id_t id)
{
    int n_errors = 0;
    int n_tests = 0;
    int retval;
    
#if TEST_REG_PART_F1
    // Test if pulse F1 settings can be set and read
    pulser_set_f1_end_switch(id, 0, 0);
    retval = pulser_read_f1_end(id);
    if (retval != 0) n_errors++;
    n_tests++;
    retval = pulser_read_f1_switch(id);
    if (retval != 0) n_errors++;
    n_tests++;

    pulser_set_f1_end_switch(id, 48000, 31000);
    retval = pulser_read_f1_end(id);
    if (retval != 48000) n_errors++;
    n_tests++;
    retval = pulser_read_f1_switch(id);
    if (retval != 31000) n_errors++;
    n_tests++;
#endif

#if TEST_REG_PART_F2
    // Test if pulse F2 settings can be set and read
    pulser_set_f2_end_switch(id, 0, 0);
    retval = pulser_read_f2_end(id);
    if (retval != 0) n_errors++;
    n_tests++;
    retval = pulser_read_f2_switch(id);
    if (retval != 0) n_errors++;
    n_tests++;

    pulser_set_f2_end_switch(id, 43000, 51000);
    retval = pulser_read_f2_end(id);
    if (retval != 43000) n_errors++;
    n_tests++;
    retval = pulser_read_f2_switch(id);
    if (retval != 51000) n_errors++;
    n_tests++;
#endif

#if TEST_REG_PART_CNT
    // Test if N pulses can be set and read
    pulser_set_f1_f2_stop_count(id, 0, 0, 0);
    retval = pulser_read_count(id);
    if (retval != 0) n_errors++;
    n_tests++;

    pulser_set_f1_f2_stop_count(id, 152, 3, 254);
    retval = pulser_read_count(id);
    if (retval != ((254 << 16) | (3 << 8) | (152 << 0))) n_errors++;
    n_tests++;
#endif

    printf("N Tests: %x, N Errors: %x\n", n_tests, n_errors);
    uart_write_flush();
}
#endif

#if TEST_RUN_PULSER_ONE_BY_ONE || TEST_RUN_ALL_PULSERS
static inline void set_testconf(void) {
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
    pulser_config(PULSER_4, &pulser0_settings);
    pulser_config(PULSER_5, &pulser1_settings);
    pulser_config(PULSER_6, &pulser2_settings);
    pulser_config(PULSER_7, &pulser3_settings);
}
#endif

#if TEST_RUN_PULSER_ONE_BY_ONE
void test_pulser_one_by_one(void) {

    set_testconf();

    for (int i = 0; i < 10; i++)
    {
        __asm__ volatile("nop");
    }

    // Enable Pulsers
    // pulser_en((1 << PULSER_0) | (1 << PULSER_1) | (1 << PULSER_2) | (1 << PULSER_3));
    pulser_en(0xFF);

    for (int id = 0; id < N_PULSERS; id++) {
        pulser_start(1 << id);
        pulser_stop(1 << id);
        for (int i = 0; i < 5; i++)
        {
            __asm__ volatile("nop");
        }
        pulser_start(1 << id);
    }
    
    // pulser_disable_all_after_done();
}
#endif

#if TEST_RUN_ALL_PULSERS
void test_pulser_run_all(void) {

    set_testconf();

    // Enable Pulsers
    // pulser_en((1 << PULSER_0) | (1 << PULSER_1) | (1 << PULSER_2) | (1 << PULSER_3));
    // pulser_stop((1 << PULSER_0) | (1 << PULSER_1) | (1 << PULSER_2) | (1 << PULSER_3));
    pulser_en(0xFF);
    pulser_stop(0xFF);

    // pulser_start((1 << PULSER_0) | (1 << PULSER_1) | (1 << PULSER_2) | (1 << PULSER_3));
    // pulser_stop((1 << PULSER_0) | (1 << PULSER_1) | (1 << PULSER_2) | (1 << PULSER_3));
    // for (int i = 0; i < 5; i++)
    // {
    //     __asm__ volatile("nop");
    // }
    pulser_start(0xFF);


    // pulser_disable_all_after_done();
}
#endif

#if TEST_RUN_ADV_TIMER
void test_adv_timer(void) {
    // Read Signature from ROM
    int topvalue = 0x300;
    int topvalue_read = timer0_get_top_value(); // Causes first APB communication
    printf("Timer Top Value before init: 0x%x\n", topvalue_read);
    timer0_init(topvalue);
    topvalue_read = timer0_get_top_value();
    printf("Timer Top Value after init: 0x%x (expexted 0x%x)\n", topvalue_read, topvalue);

    adv_timer_start(0);
    printf("Timer started\n");
    uart_write_flush();

    // Wait for the timer to reach some threshold
    int wait_until_counter = 0x200;
    while (timer0_get_counter() < wait_until_counter && wait_until_counter < topvalue)
        ;

    printf("Timer reached threshold of 0x%x!\n", wait_until_counter);
    uart_write_flush();

}
#endif

#if TEST_RUN_ADV_TIMER_INTERRUPT
void test_adv_timer_interrupt(void) {
    // Read Signature from ROM
    int topvalue = 0x10;
    int topvalue_read = timer0_get_top_value(); // Causes first APB communication
    printf("Timer Top Value before init: 0x%x\n", topvalue_read);
    timer0_init(topvalue);
    topvalue_read = timer0_get_top_value();
    printf("Timer Top Value after init: 0x%x (expexted 0x%x)\n", topvalue_read, topvalue);
    // Enable event generation for timer 0
    // printf("Enabling event generation for timer 0\n");
    // uart_write_flush();
    adv_timer_enable_event(0, 0); // Enable event generation for channel 0 of timer 0

    adv_timer_start(0);

    // printf("Timer started\n");
    // uart_write_flush();

    // Wait for the timer to reach some threshold
    int wait_until_counter = 0x200;
    while (timer0_get_counter() < wait_until_counter && wait_until_counter < topvalue)
        ;

    printf("Timer reached threshold of 0x%x!\n", wait_until_counter);
    uart_write_flush();

}
#endif
