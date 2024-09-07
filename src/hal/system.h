#ifndef __HAL_SYSTEM_H
#define __HAL_SYSTEM_H

#include "hal.h"

#define SYSTEM_SCHEDULER_TIMER_TICKS_PER_SECOND 10000

void system_init();
void system_shutdown();
uint32_t system_get_tick();
void system_disable_tick();
void system_enable_tick();
void system_disable_irq();
void system_enable_irq();

extern void (*system_handle_timer_tick)();

#endif
