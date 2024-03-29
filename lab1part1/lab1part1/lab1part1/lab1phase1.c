    

#include "alt_types.h"
#include <stdio.h>
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#define ALT_CPU_FREQ 50000000

enum mode_t { LED, SEVEN_SEG, OFF };
struct display_state {
    enum mode_t mode;
    unsigned short value;
    unsigned short counter;
};

const unsigned int seven_seg_digits[] = { 0xffffff81, 0xffffffcf, 0xffffffff };

void display(int value, int shift, mode_t mode) {
    short bit = 0x1 & (value >> shift);
    switch (mode) {
        case LED:
            IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, bit);
            break;
        case SEVEN_SEG:
            IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_RIGHT_PIO_BASE,
                seven_seg_digits[bit]);
            break;
    }
}

void display_off() {
    IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_RIGHT_PIO_BASE,
        seven_seg_digits[2]);
}


void timer_interrupt_handler(void *context, alt_u32 id) {
    volatile struct display_state *state = (volatile struct display_state*) context;
    
    // clear timer interrupt bit in status register   
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x0);
    
    if (state->counter >= 7) {
        // we just finished looping over all 8 bits
        state->mode = OFF;
        display_off();
    } else {
        state->counter++;
        display(state->value, state->counter, state->mode);
    }
}

void init_wait(int ms_to_wait, volatile struct display_state *state) {
    int timerPeriod = ALT_CPU_FREQ / 1000 * ms_to_wait;

    // initialize timer period
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, (alt_u16)timerPeriod);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, (alt_u16)(timerPeriod >> 16));

    // clear timer interrupt bit in status register   
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x0);

    // initialize timer interrupt vector
    alt_irq_register(TIMER_0_IRQ, (void*) state, timer_interrupt_handler); 

    // initialize timer control - start timer, run continuously, enable interrupts
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,
        ALTERA_AVALON_TIMER_CONTROL_ITO_MSK |
        ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
        ALTERA_AVALON_TIMER_CONTROL_START_MSK);  

    // IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0);
}

void button_interrupt_handler(void *context, alt_u32 id) {
    volatile struct display_state *state = (volatile struct display_state*) context;
    unsigned short button = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);
    
    /* Reset the Button's edge capture register. */
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0);
  
    /* 
     * Read the PIO to delay ISR exit. This is done to prevent a spurious
     * interrupt in systems with high processor -> pio latency and fast
     * interrupts.
     */
    IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);
    
    /* actually respond to the button press by altering the display */
    /* find the mode from the button press, then reset the button press */
    switch (button) {
        case 0x1:
            state->mode = LED;
            break;
        case 0x2:
            state->mode = SEVEN_SEG;
            break;
        default:
            return;
    }

    // read the value in from the switches 
    state->value = IORD_ALTERA_AVALON_PIO_DATA(SWITCH_PIO_BASE);
    state->counter = 0;
    
    /* clear out the display before displaying new data.
     * this will ensure that leds and 7seg aren't both on at the
     * same time */
    display_off();
    
    /* reset the timer, otherwise the first bit will be displayed
     * for less than a single second, since this event likely occurs
     * partway through a timer period */
    init_wait(1000, state);
    
    display(state->value, state->counter, state->mode);
}

void init_button_interrupts(volatile struct display_state *state) {
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_PIO_BASE, 0xf);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0x0);

    alt_irq_register(BUTTON_PIO_IRQ, (void*) state, 
                     button_interrupt_handler);
}


int main(void) {
    struct display_state state;
    state.mode = OFF;
    
    /* explicitly turn off any leds or seven seg displays,
     * since they retain whatever state they were in when the program starts */
    
    display_off();
    IOWR_ALTERA_AVALON_PIO_DATA(RED_LED_PIO_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LED_PIO_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_MIDDLE_PIO_BASE, 0xffff);
    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_PIO_BASE, 0xffff);          
    
    init_button_interrupts(&state);
   
    for (;;) {
        /* busy loop all day */
    }
    return 0;
}
