#pragma once

#include "fat.h"

enum playback_state {
	PLAY,
	INTERRUPT,
	DONE
};

struct playback_data {
	enum playback_state state;
	data_file file;
};

void init_button_interrupts(struct playback_data *data);
