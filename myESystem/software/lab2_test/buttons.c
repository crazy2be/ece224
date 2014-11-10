#include "buttons.h"

#include "alt_types.h"
#include <stdio.h>
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"
//#include "altera_avalon_timer_regs.h"
#include "LCD.h"

char WAV_FILETYPE[] = "WAV";

void button_interrupt_handler(void *context, alt_u32 id) {
	struct playback_data *data = (struct playback_data*) context;

    unsigned short button = IORD(BUTTON_PIO_BASE, 0); //IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);
    //printf("button: %hu\n", 0xf ^ button);

    /* Reset the Button's edge capture register. */
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0);

    /*
     * Read the PIO to delay ISR exit. This is done to prevent a spurious
     * interrupt in systems with high processor -> pio latency and fast
     * interrupts.
     */
    //IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);

    /* actually respond to the button press by altering the display */
    /* find the mode from the button press, then reset the button press */
    switch (0xf ^ button) {
        case 0x1: // button 0 - stop playback
			data->state = DONE;
            break;
        case 0x2: // button 1 - start playback
        	data->state = START;
            break;
        case 0x4: // button 2 - go to next song
        	if (search_for_filetype(WAV_FILETYPE, &data->file, 0, 1) == 0) {
				LCD_Display(data->file.Name, 1);
        	}
        	break;
        case 0x8: // button 3 - go to previous song
        	break;
        default:
        	return;
    }
}

void init_button_interrupts(struct playback_data *data) {
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_PIO_BASE, 0xf);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0x0);

	alt_irq_register(BUTTON_PIO_IRQ, (void*) data,
	                 button_interrupt_handler);
}

enum speed get_speed_from_switches() {
	return (enum speed) (0x3 & IORD(SWITCH_PIO_BASE, 0));
}
