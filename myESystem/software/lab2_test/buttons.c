#include "buttons.h"

#include "alt_types.h"
#include <stdio.h>
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"
//#include "altera_avalon_timer_regs.h"
#include "LCD.h"

char WAV_FILETYPE[] = "WAV";

void update_display(struct playback_data *data) {
	if (data->display_clean) return;
	LCD_Display(data->files[data->file_index].Name, get_speed_from_switches());
	data->display_clean = true;
}

static inline int mod(int a, int b) {
	return ((a % b) + b) % b;
}

static inline void change_song(struct playback_data *data, int amount) {
	data->file_index = mod(data->file_index + amount, data->files_len);
	data->display_clean = false;
}

void button_interrupt_handler(void *context, alt_u32 id) {
	struct playback_data *data = (struct playback_data*) context;

    unsigned short button = 0xf ^ IORD(BUTTON_PIO_BASE, 0); //IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);
    //printf("button: %hu\n", 0xf ^ button);

    // Reset the Button's edge capture register.
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0);

    /* actually respond to the button press by altering the display */
    /* find the mode from the button press, then reset the button press */
    if (button != 0x1 && data->state != DONE) {
    	return;
    }
    switch (button) {
        case 0x1: // button 0 - stop playback
			data->state = DONE;
            break;
        case 0x2: // button 1 - start playback
        	data->state = START;
            break;
        case 0x4: // button 2 - go to next song
        	change_song(data, 1);
        	break;
        case 0x8: // button 3 - go to previous song
        	change_song(data, -1);
        	break;
        default:
        	return;
    }
}

void init_button_interrupts(struct playback_data *data) {
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_PIO_BASE, 0xf);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0x0);

	alt_irq_register(BUTTON_PIO_IRQ, (void*) data, button_interrupt_handler);
}

enum speed get_speed_from_switches() {
	return (enum speed) (IORD(SWITCH_PIO_BASE, 0));
}
