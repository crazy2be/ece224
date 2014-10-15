#include <io.h>
#include <stdio.h>

#include "sys/alt_irq.h"
#include "ece224_egm.h"

#define MAX_PULSE_COUNT 100

void occasional_polling_main(int grain_size) {
    int pulses;
    int state = 0;

    for (pulses = 0; pulses < 2 * MAX_PULSE_COUNT; pulses++) {
        while (IORD(PIO_PULSE_BASE, 0) == state) {
            background(grain_size);
        }
        state = !state;
        IOWR(PIO_RESPONSE_BASE, 0, state);
    }
}

void pulse_interrupt_handler(void *context, alt_u32 id) {
    // get the level of the pulse, so we know what to respond
    int value = IORD(PIO_PULSE_BASE, 0);
    
    /* Reset the Button's edge capture register. */
    IOWR(PIO_PULSE_BASE, 3, 0);
    
    /* Respond to the pulse with the same value received */
    IOWR(PIO_RESPONSE_BASE, 0, value);
    
    /* Count that the pulse was received */
    (*(volatile int*)context)++;
}

void init_pulse_interrupts(volatile int *pulse_count) {
    IOWR(PIO_PULSE_BASE, 2, 0xf);
    IOWR(PIO_PULSE_BASE, 3, 0x0);

    alt_irq_register(PIO_PULSE_IRQ, (void*) pulse_count, pulse_interrupt_handler);
}

void interrupt_main(int grain_size) {
    volatile int pulse_count = 0;

    init_pulse_interrupts(&pulse_count);

    while (pulse_count < 2 * MAX_PULSE_COUNT) {
        background(grain_size);
    }
}

#define USE_INTERRUPT_SYNCHRONIZATION

int main(void) {
    int period;
    int duty_cycle;
    int grain_size;
    printf("technique, period_us, duty_cycle_percent, latency_resolution_ns, "
           "number_of_missed_pulses, max_latency_1024th_of_a_period, "
           "max_latency_us, task_units_processed\n");
    for (period = 2; period <= 14; period += 2) {
        for (duty_cycle = 2; duty_cycle <= 14; duty_cycle += 2) {
            for (grain_size = 50; grain_size <= 500; grain_size += 50) {
                #if 0
                printf("Running experiment with period = %d, duty_cycle = %d, "
                       "grain_size = %d, sync=interrupts\n", period,
                       duty_cycle, grain_size);
                       #endif
                       printf("interrupts, ");
                init(period, duty_cycle);
                interrupt_main(grain_size);
                finalize();
                
                #if 0
                printf("Running experiment with period = %d, duty_cycle = %d, "
                       "grain_size = %d, sync=polling\n", period,
                       duty_cycle, grain_size);
                       #endif
                       printf("polling, ");
                init(period, duty_cycle);
                occasional_polling_main(grain_size);
                finalize();
            }
        }
    }
    printf("done!");
    return 0;
}