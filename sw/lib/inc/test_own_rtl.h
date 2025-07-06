#ifndef TEST_OWN_RTL_H
#define TEST_OWN_RTL_H

#include "util.h"

#if TEST_REG_PART_F1 || TEST_REG_PART_F2 || TEST_REG_PART_CNT || TEST_RUN_ALL_PULSERS || TEST_RUN_PULSER_ONE_BY_ONE
    #include "pulser.h"
    #ifdef __cplusplus
    extern "C"
    {
    #endif

        void test_pulser_regs(pulser_id_t id);
        void test_pulser_run_all(void);
        void test_pulser_one_by_one(void);

    #ifdef __cplusplus
    } // extern "C"
    #endif
#endif

#if TEST_RUN_ADV_TIMER || TEST_RUN_ADV_TIMER_INTERRUPT
    #include "adv_timer.h"
    #ifdef __cplusplus
    extern "C"
    {
    #endif
        void test_adv_timer(void);
        void test_adv_timer_interrupt(void);

    #ifdef __cplusplus
    } // extern "C"
    #endif
#endif

#if TEST_READ_ROM
    #ifdef __cplusplus
    extern "C"
    {
    #endif
        void test_read_rom(void);

    #ifdef __cplusplus
    } // extern "C"
    #endif
#endif

#endif // TEST_OWN_RTL_H
