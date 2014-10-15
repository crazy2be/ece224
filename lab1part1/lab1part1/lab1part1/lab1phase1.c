    

#include "alt_types.h"
#include <stdio.h>
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#define ALT_CPU_FREQ 50000000

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
    volatile int *timer_counter = (volatile int*) context;
    (*timer_counter)--;
    // clear timer interrupt bit in status register   
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x0);
}

void init_wait(int ms_to_wait, volatile int *timer_counter) {
    int timerPeriod = ALT_CPU_FREQ / 1000 * ms_to_wait;

    // initialize timer period
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, (alt_u16)timerPeriod);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, (alt_u16)(timerPeriod >> 16));

    // clear timer interrupt bit in status register   
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x0);

    // initialize timer interrupt vector
    alt_irq_register(TIMER_0_IRQ, (void*) timer_counter, timer_interrupt_handler); 

    // initialize timer control - start timer, run continuously, enable interrupts
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,
        ALTERA_AVALON_TIMER_CONTROL_ITO_MSK |
        ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
        ALTERA_AVALON_TIMER_CONTROL_START_MSK);  

    // IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0);
}

enum mode_t { LED, SEVEN_SEG, OFF };
const unsigned seven_seg_digits[] = { 0xffffff81, 0xffffffcf, 0xffffffff };

void display(int value, mode_t mode) {
    short bit = 0x1 & value;
    switch (mode) {
        case LED:
            IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, bit);
            break;
        case SEVEN_SEG:
            IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_RIGHT_PIO_BASE,
                seven_seg_digits[bit]);
            break;
        case OFF:
            IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 0);
            IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_RIGHT_PIO_BASE,
                seven_seg_digits[2]);
            break;
    }
} 

int main(void) {
    enum mode_t mode = OFF;
    int value;
    int counter;
    volatile int button_edge_value;
    volatile int timer_counter = 1;
    
    /* explicitly turn off any leds or seven seg displays,
     * since they retain whatever state they were in when the program starts */
    
    IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(RED_LED_PIO_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LED_PIO_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_RIGHT_PIO_BASE, seven_seg_digits[2]);
    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_MIDDLE_PIO_BASE, 0xffff);
    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_PIO_BASE, 0xffff);          
    
    init_button_interrupts(&button_edge_value);
   
    for (;;) {
        /* Note: we don't care about bouncing, since if we detect two presses,
         * we'll just restart the cycle very quickly.
         * This is too fast to be discernable, so we don't need to worry about this. */
        if (button_edge_value == 0x1 || button_edge_value == 0x2) {
            /* find the mode from the button press, then reset the button press */
            mode = (button_edge_value == 0x1) ? LED : SEVEN_SEG;
            button_edge_value = 0;
            
            value = IORD_ALTERA_AVALON_PIO_DATA(SWITCH_PIO_BASE);
            counter = 0;
            
            /* clear out the display before displaying new data.
             * this will ensure that leds and 7seg aren't both on at the
             * same time */
            display(0, OFF);
            
            /* reset the timer, otherwise the first bit will be displayed
             * for less than a single second, since this event likely occurs
             * partway through a timer period */
            timer_counter = 1;
            init_wait(1000, &timer_counter);
        } else if (mode != OFF && timer_counter <= 0) {
            if (counter >= 7) {
                // we just finished looping over all 8 bits
                mode = OFF;
            } else {
                // reset the timer, and display the next bit
                timer_counter = 1;
                value >>= 1;
                counter++;
            }
        } else {
            // don't update the display if nothing happened
            continue;
        }
        display(value, mode);
    }
    return 0;
}
