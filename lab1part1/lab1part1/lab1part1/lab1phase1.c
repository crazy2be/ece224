

#include "alt_types.h"
//#include <stdio.h>
//#include <unistd.h>
//#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"


void button_interrupt_handler(void *context, alt_u32 id) {
    volatile int *ret = (volatile int*) context;
    *ret = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);
    /* Reset the Button's edge capture register. */
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0);
  
    /* 
     * Read the PIO to delay ISR exit. This is done to prevent a spurious
     * interrupt in systems with high processor -> pio latency and fast
     * interrupts.
     */
    IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);
}

void init_button_interrupts(volatile int *ret) {
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_PIO_BASE, 0xf);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0x0);

    alt_irq_register(BUTTON_PIO_IRQ, (void*) ret, 
                     button_interrupt_handler);
}

void timer_interrupt_handler(void *context, alt_u32 id) {
    (*(volatile int*) context)--;
}

void wait(int ms_to_wait) {
    volatile int i = ms_to_wait;
    // turn on timer interrupt handler
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(TIMER_0_PIO_BASE, 0xf);
    alt_irq_register(TIMER_0_PIO_IRQ, &i, timer_interrupt_handler);
    // timer fires an event every 1ms
    while (i > 0);
    // disable timer interrupts again
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(TIMER_0_PIO_BASE, 0x0);
}

int main(void) {
    //volatile alt_16 led = 0x00;
    volatile int button_edge_value;
    init_button_interrupts(&button_edge_value);
    
    while (1) {
        while (button_edge_value == 0);
        button_edge_value = 0;
        IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 0x1);
        wait(1000);
        IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 0x0);
        //volatile int sw = IORD_ALTERA_AVALON_PIO_DATA(SWITCH_PIO_BASE);
        //IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LED_PIO_BASE, sw);
    }
    return 0;
}