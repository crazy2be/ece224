#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "fat.h"

struct file_stream {
	int *cluster_chain;
	uint16_t cluster_chain_len;
	uint32_t sector_index;
	data_file *file;
};

void fs_init(struct file_stream *fs, data_file *f);

void fs_free(struct file_stream *fs);

// returns the number of bytes copied, -1 on error
int fs_read(struct file_stream *fs, uint8_t *buf);

// returns false on error
bool fs_seek(struct file_stream *fs, uint32_t sector);
