#include "alt_types.h"
#include <stdio.h>
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include "SD_Card.h"
#include "fat.h"
#include "stdio.h"

enum mode_t { LED, SEVEN_SEG, OFF };
const unsigned int seven_seg_digits[] = { 0xffffff81, 0xffffffcf, 0xffffffff };

void display(mode_t mode, int value) {
    switch (mode) {
	case LED:
		IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, value);
		break;
	case SEVEN_SEG:
		// TODO: Fix this!
		//IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_RIGHT_PIO_BASE,
		//	seven_seg_digits[bit]);
		break;
    }
}

int main(void) {
	data_file df;
	printf("About to search for filename\n");
	display(LED, 0x1);
	search_for_filetype("wav", &df, 0, 1);
	display(LED, 0x2);
	printf("Done searching for filename\n");
	display(LED, 0x3);

	printf("Found file with name: %s\n", df.Name);
}

