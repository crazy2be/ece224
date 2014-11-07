#include "altera_avalon_pio_regs.h"
#include "fat.h"
#include "file_stream.h"
#include "Open_I2C.h"
#include "sd.h"
#include <stdio.h>
#include <stdlib.h>
#include <system.h>
#include "wm8731.h"

enum mode_t { LED, SEVEN_SEG, OFF };
const unsigned int seven_seg_digits[] = { 0xffffff81, 0xffffffcf, 0xffffffff };
const uint16_t PRESCALE = 0x000a;

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
	if (SD_card_init() != 0) {
		printf("Error initializing SD card\n");
		return 1;
	} else if (init_mbr() != 0) {
		printf("Error initializing MBR\n");
		return 1;
	} else if (init_bs() != 0) {
		printf("Error initializing boot sector\n");
		return 1;
	}
	// info_bs();
	data_file df;
	search_for_filetype("WAV", &df, 0, 1);

	printf("Found file with name: %s\n", df.Name);

	//I2C_Init(PRESCALE);
	init_audio_codec();

	printf("Done all configuration\n");

	uint16_t tmp = 0;
	while (1) {
		IOWR(AUDIO_0_BASE, 0, tmp);

		tmp = rand() % 0xffff;
	}

	return 0;
}

