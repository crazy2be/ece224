

#include "alt_types.h"
#include <stdio.h>
//#include <inttypes.h>
//#include <unistd.h>
//#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

#define ALT_CPU_FREQ 50000000

void inc_green_leds() {
    static unsigned cur = 0;
    IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LED_PIO_BASE, cur++);
    volatile int i = 1000;
    while (i--) {}
}
void inc_red_leds() {
    static unsigned cur = 0;
    IOWR_ALTERA_AVALON_PIO_DATA(RED_LED_PIO_BASE, cur++);
    volatile int i = 1000;
    while (i--) {}
}
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

volatile static int timer_counter = 0;
void timer_interrupt_handler(void *context, alt_u32 id) {
    timer_counter--;
    inc_red_leds();
    //(*(volatile int*) context)--;
    
   // clear timer interrupt bit in status register   
   IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x0);
}

void wait(int ms_to_wait) {
    timer_counter = ms_to_wait;
    while (timer_counter > 0) {
        inc_green_leds();
    }
}

void wait_init() {
   int timerPeriod = ALT_CPU_FREQ / 1000;

   // initialize timer period
   IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, (alt_u16)timerPeriod);
   IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, (alt_u16)(timerPeriod >> 16));

   // clear timer interrupt bit in status register   
   IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0x0);

   // initialize timer interrupt vector
   alt_irq_register(TIMER_0_IRQ, (void*)0, timer_interrupt_handler); 

   // initialize timer control - start timer, run continuously, enable interrupts
   IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,
        ALTERA_AVALON_TIMER_CONTROL_ITO_MSK |
        ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
        ALTERA_AVALON_TIMER_CONTROL_START_MSK);
    
}

int main(void) {
    //volatile alt_16 led = 0x00;
    //volatile int button_edge_value = 0;
    //init_button_interrupts(&button_edge_value);
    int i = 1;
    wait_init();
    
    while (1) {
        //while (button_edge_value == 0);
        //button_edge_value = 0;
        IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, i++);
        wait(1000);
        //volatile int sw = IORD_ALTERA_AVALON_PIO_DATA(SWITCH_PIO_BASE);
        //IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LED_PIO_BASE, sw);
    }
    return 0;
}
