#pragma once

#include "fat.h"

enum playback_state {
	START,
	PLAYING,
	DONE
};

enum speed {
	NORMAL,
	DOUBLE,
	HALF,
	REVERSE,
	DELAY,
};

struct playback_data {
	enum playback_state state;
	data_file *files;
	int file_index;
	int files_len;
	bool display_clean;
};

void update_display(struct playback_data *data);
void init_button_interrupts(struct playback_data *data);
enum speed get_speed_from_switches();
