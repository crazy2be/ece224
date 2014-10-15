#include <io.h>
#include "sys/alt_irq.h"
#include "ece224_egm.h"


#define MAX_PULSE_COUNT 100

void occasional_polling_main(void) {
    int pulses;
    int state = 0;
    const int grain_size = 20;

    for (pulses = 0; pulses < 100; pulses++) {
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

void interrupt_main(void) {
    const int grain_size = 120;
    volatile int pulse_count = 0;

    init_pulse_interrupts(&pulse_count);

    while (pulse_count < MAX_PULSE_COUNT) {
        background(grain_size);
    }
}

#define USE_INTERRUPT_SYNCHRONIZATION

int main(void) {
    init(6, 8);

#ifdef USE_INTERRUPT_SYNCHRONIZATION
    interrupt_main();
#else
    occasional_polling_main();
#endif

    finalize();
    return 0;
}