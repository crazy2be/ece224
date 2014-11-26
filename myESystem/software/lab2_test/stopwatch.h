#pragma once

#include "altera_avalon_timer_regs.h"

#include <stdio.h>
#include <inttypes.h>

static inline void timer_init() {
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE, 0xffff);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_1_BASE, 0xffff);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE,
        ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
        ALTERA_AVALON_TIMER_CONTROL_START_MSK);
}

static inline uint32_t timer_sample() {
	// 10 us resolution. Note: timer counts **DOWN**!
	IOWR_ALTERA_AVALON_TIMER_SNAPL(TIMER_1_BASE, 0); // Magic
	return (IORD_ALTERA_AVALON_TIMER_SNAPH(TIMER_1_BASE) << 16) | IORD_ALTERA_AVALON_TIMER_SNAPL(TIMER_1_BASE);
}

struct stopwatch {
	uint32_t max;
	uint32_t min;
	uint64_t sum;
	uint32_t num;
	uint32_t prev;
};

static void stopwatch_reset(struct stopwatch *sw) {
	*sw = (struct stopwatch) {};
}
static inline void stopwatch_start(struct stopwatch *sw) {
	sw->prev = timer_sample();
}
static inline void stopwatch_lap(struct stopwatch *sw) {
	uint32_t now = timer_sample();
	uint32_t val = sw->prev - now;
	sw->prev = now;
	//if (val > TIMER_1_TICKS_PER_SEC) val = TIMER_1_TICKS_PER_SEC;
	if (val > sw->max) sw->max = val;
	if (val < sw->min) sw->min = val;
	sw->sum += val;
	sw->num++;
}
static inline void stopwatch_stop(struct stopwatch *sw) {
	stopwatch_lap(sw);
}
static void stopwatch_print(struct stopwatch *sw) {
	printf("%"PRIu32" laps, min %"PRIu32" avg %"PRIu64" max %"PRIu32" sum %"PRIu64", per sec %"PRIu64"\n",
			sw->num, sw->min, sw->sum/(uint64_t)sw->num, sw->max, sw->sum, (uint64_t)TIMER_1_TICKS_PER_SEC);
}
