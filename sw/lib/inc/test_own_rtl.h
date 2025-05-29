#ifndef TEST_OWN_RTL_H
#define TEST_OWN_RTL_H

#include "util.h"
#include "pulser.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void test_read_rom(void);
    void test_pulser_regs(pulser_id_t id);
    void test_pulser_run_all(void);
    void test_adv_timer(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TEST_OWN_RTL_H
