#include "buttons.h"

#include "altera_avalon_pio_regs.h"
#include "buttons.h"
#include "fat.h"
#include "file_stream.h"
#include "LCD.h"
#include "Open_I2C.h"
#include "sd.h"
#include <stdbool.h>
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

inline uint16_t attenuate(uint16_t level) {
	return (uint16_t) ((int16_t) level / 256);
}

void write_to_7seg(uint32_t x) {
	const uint8_t pat[] = {
			0x01, // 0
			0x4f, // 1
			0x12, // 2
			0x06, // 3
			0x4c, // 4
			0x24, // 5
			0x20, // 6
			0x0f, // 7
			0x00, // 8
			0x04, // 9
			0x08, // A
			0x60, // b
			0x31, // C
			0x42, // d
			0x30, // E
			0x38  // F
	};
	uint32_t p = 0;
	for (int i = 3; i >= 0; i--) {
		p = (p << 8) | pat[(x >> (4 * i)) & 0xf];
	}
	IOWR(SEVEN_SEG_RIGHT_PIO_BASE, 0, p);
}

void play_audio(struct file_stream *fs, enum speed speed, volatile enum playback_state *state) {
	uint8_t buf[BPB_BytsPerSec];
	int bytes_read;
	int i = 12 + 24 + 8; // initially skip header
	int increment;
	int duplicates;

	switch (speed) {
	case NORMAL:
		increment = 1;
		duplicates = 1;
		break;
	case DOUBLE:
		increment = 2;
		duplicates = 1;
		break;
	case HALF:
		increment = 1;
		duplicates = 2;
		break;
	case REVERSE:
		// TODO
		return;
	}

	while (*state == PLAYING && (bytes_read = fs_read(fs, buf)) != -1) {
		for ( ; i < bytes_read; i += 2) {
			uint16_t part = attenuate((buf[i + 1] << 8) | buf[i]);
			while(IORD(AUD_FULL_BASE, 0)); //wait until the FIFO is not full
			// sector buffer array into the single 16-bit variable tmp
			IOWR(AUDIO_0_BASE, 0, part);
		}
		i = 0;
	}
}

void init(struct playback_data *data) {
	if (SD_card_init() != 0) {
		printf("Error initializing SD card\n");
	} else if (init_mbr() != 0) {
		printf("Error initializing MBR\n");
	} else if (init_bs() != 0) {
		printf("Error initializing boot sector\n");
	} else {
		 // cannot fail
		LCD_Init();
		init_audio_codec();
		init_button_interrupts(data);
		printf("Done all configuration\n");
		return;
	}
	exit(1);
}

void loop(volatile struct playback_data *data) {
	for (;;) {
		if (data->state == START) {
			data->state = PLAYING;
			data_file current = data->file;
			struct file_stream fs;
			fs_init(&fs, &current);
			play_audio(&fs, get_speed_from_switches(), &data->state);
			fs_free(&fs);
		}
	}
}

int main(void) {
	struct playback_data data;
	init(&data);
	loop(&data);
	return 0;
}

