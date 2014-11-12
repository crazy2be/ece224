#include "buttons.h"

#include "altera_avalon_pio_regs.h"
#include <assert.h>
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

inline int16_t get_sample(uint8_t *p, int increment) {
	return (int16_t) ((p[1] << 8) | p[0]);

//	int32_t sample = 0;
//	for (int i = 0; i < increment; i ++) {
//		sample += (int16_t) ((p[4*i + 1] << 8) | p[4*i]);
//	}
//
//    return sample / increment;
}

inline void play_sample(int16_t sample) {
    while(IORD(AUD_FULL_BASE, 0)); //wait until the FIFO is not full
    // sector buffer array into the single 16-bit variable tmp
    IOWR(AUDIO_0_BASE, 0, (uint16_t) sample);
}

inline void read_and_play_sample(uint8_t *buf, int increment, int duplicates) {
    int16_t left = attenuate(get_sample(buf, increment));
    int16_t right = attenuate(get_sample(buf + 2, increment));
    for (int j = 0; j < duplicates; j++) {
        play_sample(left);
        play_sample(right);
    }
}

void play_audio(struct file_stream *fs, enum speed speed, volatile enum playback_state *state) {
	uint8_t buf[BPB_BytsPerSec];
	int bytes_read;
	int i = 12 + 24 + 8; // initially skip header
	int increment = 1;
	int block_increment = 1;
	int duplicates = 1;

	switch (speed) {
	case NORMAL:
		break;
	case DOUBLE:
		increment = 2;
		break;
	case HALF:
		duplicates = 2;
		break;
	case REVERSE:
        assert(fs_seek_rel(fs, -1));
        block_increment = -1;
        // much of the logic is a special case here
		break;
	case DELAY:
		printf("TODO");
		return;
	default:
		return;
	}

	while (*state == PLAYING && (bytes_read = fs_read(fs, buf)) != -1) {
		write_to_7seg(fs->sector_index);
        if (speed == REVERSE) {
            for (int j = bytes_read - 4; j >= i; j -= 4) {
                read_and_play_sample(buf + j, increment, duplicates);
            }
        } else {
            for ( ; i < bytes_read; i += 4 * increment) {
                read_and_play_sample(buf + i, increment, duplicates);
            }
        }
        if (fs_seek_rel(fs, block_increment)) {
        	break;
        }
		i = 0;
	}
}

void read_filenames(struct playback_data *data) {
	int num_files = 0;
	data_file tmp;
	assert(!search_for_filetype("WAV", &tmp, 0, 1));
	uint32_t first_file_first_cluster = tmp.FirstSector;
	do {
		num_files++;
		printf("Found file %s at sector %d\n", tmp.Name, tmp.FirstSector);
		assert(!search_for_filetype("WAV", &tmp, 0, 1));
		printf("Found file %s at sector %d\n", tmp.Name, tmp.FirstSector);
	} while (tmp.FirstSector != first_file_first_cluster);

	data->files = malloc(num_files * sizeof(data_file));
	assert(data->files);
	data->files[0] = tmp;
	for (int i = 1; i < num_files; i++) {
		assert(!search_for_filetype("WAV", &data->files[i], 0, 1));
	}
	data->files_len = num_files;
	data->file_index = 0;
	printf("Found %d files\n", num_files);
}
void init(struct playback_data *data) {
	if (SD_card_init() != 0) {
		printf("Error initializing SD card\n");
	} else if (init_mbr() != 0) {
		printf("Error initializing MBR\n");
	} else if (init_bs() != 0) {
		printf("Error initializing boot sector\n");
	} else {
		// cannot fail, or else we do :(.
		read_filenames(data);
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
			struct file_stream fs;
			// Buffering...
			LCD_File_Buffering(data->files[data->file_index].Name);
			data->display_clean = false;
			fs_init(&fs, &data->files[data->file_index]);
			// Playing "DEADJIM"
			update_display((struct playback_data*) data);
			play_audio(&fs, get_speed_from_switches(), &data->state);
			fs_free(&fs);
			data->state = DONE;
		} else {
			update_display((struct playback_data*) data);
		}
	}
}

int main(void) {
	struct playback_data data = {};
	data.state = DONE;
	init(&data);
	loop(&data);
	return 0;
}

