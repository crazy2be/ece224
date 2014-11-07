#pragma once
#include <inttypes.h>

uint8_t SD_read_lba(uint8_t *, uint32_t, uint32_t);
uint8_t SD_card_init(void);
