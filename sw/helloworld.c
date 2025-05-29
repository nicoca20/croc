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
#include "test_own_rtl.h"

int main()
{

    uart_init(); // setup the uart peripheral

    // simple printf support (only prints text and hex numbers)
    // printf("Hello World!\n");
    // uart_write_flush();

#if TEST_REG_PART_F1 || TEST_REG_PART_F2 || TEST_REG_PART_CNT
    for (int id = 0; id < N_PULSERS; id++) {
        printf("Pulser %x\n", id);
        uart_write_flush();
        test_pulser_regs(id);
    }
#endif

#if TEST_READ_ROM
    test_read_rom();
#endif

#if TEST_RUN_ALL_PULSERS
    test_pulser_run_all();
#endif


#if TEST_RUN_ADV_TIMER
    test_adv_timer();
#endif

    return 1;
}
