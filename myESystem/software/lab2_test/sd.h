#pragma once
#include <inttypes.h>

// buf must be 512 bytes long
int SD_read_lba(uint8_t *buf, uint32_t lba);
int SD_card_init(void);
