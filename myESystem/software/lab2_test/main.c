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
#include "stopwatch.h"

enum mode_t { LED, SEVEN_SEG, OFF };
const unsigned int seven_seg_digits[] = { 0xffffff81, 0xffffffcf, 0xffffffff };
const uint16_t PRESCALE = 0x000a;
const uint16_t SAMPLE_FREQ = 44100; // 44.1 KHz

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

static inline int16_t attenuate(int16_t level) {
	return level;
}

static inline int16_t get_sample(uint8_t *p, int increment) {
	return (int16_t) ((p[1] << 8) | p[0]);
}

static struct stopwatch sample_wait_stopwatch = {};
inline void play_sample(int16_t sample) {
	//stopwatch_start(&sample_wait_stopwatch);
    while (IORD(AUD_FULL_BASE, 0)) {} //wait until the FIFO is not full
    //stopwatch_stop(&sample_wait_stopwatch);
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

#include "ring_buffer.h"

void play_audio_delay(struct file_stream *fs, volatile enum playback_state *state) {
	uint8_t buf[BPB_BytsPerSec];
	const int num_seconds = 5;
	struct ring_buffer *right_buf = ring_buffer_new(SAMPLE_FREQ*num_seconds + 2);
	int sample = 0;
	int i = 12 + 24 + 8; // initially skip header

	timer_init();
	struct stopwatch sw = {};
	stopwatch_start(&sw);

	do {
		int bytes_read = fs_read(fs, buf);
		for (; i < BPB_BytsPerSec; i += 2 * sizeof(int16_t)) {
			if (i < bytes_read) {
				int16_t left = attenuate(get_sample(buf + i, 1));
				int16_t right = attenuate(get_sample(buf + i + 2, 1));
				play_sample(left);
				ring_buffer_put(right_buf, right);
			} else {
				play_sample(0);
			}
			if (sample >= SAMPLE_FREQ && ring_buffer_len(right_buf)) {
				play_sample(ring_buffer_take(right_buf));
			} else {
				play_sample(0);
			}
			sample++;
		}

		i = 0;

		stopwatch_lap(&sw);
	} while (*state == PLAYING && ring_buffer_len(right_buf));
	stopwatch_print(&sw);
}

void play_audio_reverse(struct file_stream *fs, volatile enum playback_state *state) {
	uint8_t buf[BPB_BytsPerSec];
	int bytes_read;
	int i = 12 + 24 + 8; // initially skip header
	fs_seek_end(fs);

	while (*state == PLAYING && (bytes_read = fs_readr(fs, buf)) != -1) {
			write_to_7seg(fs->sector_index);

			for (int j = bytes_read - 4; j >= i; j -= 4) {
				read_and_play_sample(buf + j, 1, 1);
			}
			i = 0;
		}
}

void play_audio_normal(struct file_stream *fs, int increment, int duplicates, volatile enum playback_state *state) {
	uint8_t buf[BPB_BytsPerSec];
	int bytes_read;
	int i = 12 + 24 + 8; // initially skip header

	timer_init();
	struct stopwatch sw = {};
	stopwatch_start(&sw);
	struct stopwatch file_read = {};
	struct stopwatch samples_play = {};
	stopwatch_reset(&sample_wait_stopwatch);

	while (*state == PLAYING) {
		stopwatch_start(&file_read);
		bytes_read = fs_read(fs, buf);
		stopwatch_stop(&file_read);
		if (bytes_read < 0) break;

		write_to_7seg(fs->sector_index);
		stopwatch_start(&samples_play);
		for ( ; i < bytes_read; i += 4 * increment) {
			read_and_play_sample(buf + i, increment, duplicates);
		}
		stopwatch_stop(&samples_play);
		i = 0;
		stopwatch_lap(&sw);
	}
	stopwatch_print(&sw);
	stopwatch_print(&file_read);
	stopwatch_print(&samples_play);
	stopwatch_print(&sample_wait_stopwatch);
}

void play_audio(struct file_stream *fs, enum speed speed, volatile enum playback_state *state) {
	init_audio_codec();

	// Reset Device
	I2C_Send(0xFE, 0, 0); //Reset MSB
	I2C_Send(0x00, 0, 1); //Write 0 to Reset LSB
	switch (speed) {
	case NORMAL:
		play_audio_normal(fs, 1, 1, state);
		break;
	case DOUBLE:
		play_audio_normal(fs, 2, 1, state);
		break;
	case HALF:
		play_audio_normal(fs, 1, 2, state);
		break;
	case REVERSE:
		play_audio_reverse(fs, state);
		break;
	case DELAY:
		play_audio_delay(fs, state);
		break;
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
