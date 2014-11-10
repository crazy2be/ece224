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
	REVERSE
};

struct playback_data {
	enum playback_state state;
	data_file file;
};

void init_button_interrupts(struct playback_data *data);
enum speed get_speed_from_switches();
