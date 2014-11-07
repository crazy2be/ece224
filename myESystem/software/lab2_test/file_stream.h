#pragma once

#include <stdint.h>

struct file_stream {
	int *cluster_chain;
	uint16_t cluster_chain_len;
};
