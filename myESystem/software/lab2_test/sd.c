#include "sd.h"

#if 1
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#else
// Dummy definitions so that I can compile this without needing to use
// eclipse.
#define IORD(a,b) true
#define IOWR(a,b,c) true
#define SD_CMD_BASE 0
#define SD_DAT_BASE 0
#define SD_CLK_BASE 0
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

//-------------------------------------------------------------------------
//  SD Card Set I/O Direction
#define SD_CMD_IN   IOWR(SD_CMD_BASE, 1, 0)
#define SD_CMD_OUT  IOWR(SD_CMD_BASE, 1, 1)
#define SD_DAT_IN   IOWR(SD_DAT_BASE, 1, 0)
#define SD_DAT_OUT  IOWR(SD_DAT_BASE, 1, 1)
//  SD Card Output High/Low
#define SD_CMD_LOW  IOWR(SD_CMD_BASE, 0, 0)
#define SD_CMD_HIGH IOWR(SD_CMD_BASE, 0, 1)
#define SD_DAT_LOW  IOWR(SD_DAT_BASE, 0, 0)
#define SD_DAT_HIGH IOWR(SD_DAT_BASE, 0, 1)
#define SD_CLK_LOW  IOWR(SD_CLK_BASE, 0, 0)
#define SD_CLK_HIGH IOWR(SD_CLK_BASE, 0, 1)
//  SD Card Input Read
#define SD_TEST_CMD IORD(SD_CMD_BASE, 0)
#define SD_TEST_DAT IORD(SD_DAT_BASE, 0)

//-------------------------------------------------------------------------
static bool sd_card_ready = false;
// Stores system registers after response_R(param) is used
static uint8_t response_buffer[20];
static uint8_t RCA[2];

//-------------------------------------------------------------------------
//SD Card Commands

// CMD0 Resets all cards to Idle State.
static const uint8_t cmd0[5] = { 0x40, 0x00, 0x00, 0x00, 0x00 };
// CMD55 Indicates to the card that the next command is an application specific
// command rather than a standard command
static const uint8_t cmd55[5] = { 0x77, 0x00, 0x00, 0x00, 0x00 };
// CMD2 Asks any card to send their CID numbers on the CMD line. (Any card
// that is connected to the host will respond.)
static const uint8_t cmd2[5] = { 0x42, 0x00, 0x00, 0x00, 0x00 };
// CMD3 Asks the card to publish a new relative address (RCA).
static const uint8_t cmd3[5] = { 0x43, 0x00, 0x00, 0x00, 0x00 };
// CMD7 Command toggles a card between the Stand-by and Transfer states
// or between the Programming and Disconnect state.
static const uint8_t cmd7[5] = { 0x47, 0x00, 0x00, 0x00, 0x00 };
// CMD9 Addressed card sends its card-specific data (CSD) on the CMD line.
static const uint8_t cmd9[5] = { 0x49, 0x00, 0x00, 0x00, 0x00 };
// CMD16 Selects a block length (in bytes) for all following block commands
// (both read and write).
static const uint8_t cmd16[5] = { 0x50, 0x00, 0x00, 0x02, 0x00 };
// CMD17 Reads a block of the size selected by the SET_BLOCKLEN command.
static const uint8_t cmd17[5] = { 0x51, 0x00, 0x00, 0x00, 0x00 };
// ACMD6 Defines the data bus width (00=1bit or 10=4 bits bus) to be used
// for data transfer.
static const uint8_t acmd6[5] = { 0x46, 0x00, 0x00, 0x00, 0x02 };
// ACMD41 Asks the accessed card to send its operating condition register (OCR)
// con tent in the response on the CMD line.
static const uint8_t acmd41[5] = { 0x69, 0x0f, 0xf0, 0x00, 0x00 };
// ACMD51 Reads the SD Configuration Register (SCR).
static const uint8_t acmd51[5] = { 0x73, 0x00, 0x00, 0x00, 0x00 };

static void Ncr(void) {
	SD_CMD_IN;
	SD_CLK_LOW;
	SD_CLK_HIGH;
	SD_CLK_LOW;
	SD_CLK_HIGH;
}

static void Ncc(void) {
	for (int i = 0; i < 8; i++) {
		SD_CLK_LOW;
		SD_CLK_HIGH;
	}
}

static uint8_t response_R(uint8_t s) {
	uint8_t a = 0, b = 0, c = 0, r = 0;
	for (int attempt = 0;; attempt++) {
		SD_CLK_LOW;
		SD_CLK_HIGH;
		if (!(SD_TEST_CMD)) break;
		if (attempt > 100) return 2;
	}
	uint8_t crc = 0;
	int j = 6;
	if (s == 2) {
		j = 17;
	}
	for (int k = 0; k < j; k++) {
		c = 0;
		if (k > 0) { //for crc culcar
			b = response_buffer[k - 1];
		}
		for (int i = 0; i < 8; i++) {
			SD_CLK_LOW;
			if (a > 0) {
				c <<= 1;
			} else {
				i++;
			}
			a++;
			SD_CLK_HIGH;
			if (SD_TEST_CMD) {
				c |= 0x01;
			}
			if (k > 0) {
				crc <<= 1;
				if ((crc ^ b) & 0x80) {
					crc ^= 0x09;
				}
				b <<= 1;
				crc &= 0x7f;
			}
		}
		if (s == 3) {
			if (k == 1 && (!(c & 0x80))) {
				r = 1;
			}
		}
		response_buffer[k] = c;
	}
	if (s == 1 || s == 6) {
		if (c != ((crc << 1) + 1)) {
			r = 2;
		}
	}
	return r;
}

static uint8_t send_cmd(const uint8_t *in) {
	uint8_t b, crc = 0;
	SD_CMD_OUT;
	for (int i = 0; i < 5; i++) {
		b = in[i];
		for (int j = 0; j < 8; j++) {
			SD_CLK_LOW;
			if (b & 0x80) {
				SD_CMD_HIGH;
			} else {
				SD_CMD_LOW;
			}
			crc <<= 1;
			SD_CLK_HIGH;
			if ((crc ^ b) & 0x80)
				crc ^= 0x09;
			b <<= 1;
		}
		crc &= 0x7f;
	}
	crc = ((crc << 1) | 0x01);
	b = crc;
	for (int j = 0; j < 8; j++) {
		SD_CLK_LOW;
		if (crc & 0x80) {
			SD_CMD_HIGH;
		} else {
			SD_CMD_LOW;
		}
		SD_CLK_HIGH;
		crc <<= 1;
	}
	return b;
}

int SD_card_init(void) {
	SD_CMD_OUT;
	SD_DAT_IN;
	SD_CLK_HIGH;
	SD_CMD_HIGH;
	SD_DAT_LOW;
	for (int x = 0; x < 40; x++) {
		Ncr();
	}
	send_cmd(cmd0);
	do {
		for (int x = 0; x < 40; x++) {}
		Ncc();
		send_cmd(cmd55);
		Ncr();
		// response too long or crc error
		if (response_R(1) > 1) return -1;
		Ncc();
		send_cmd(acmd41);
		Ncr();
	} while (response_R(3) == 1);
	Ncc();
	send_cmd(cmd2);
	Ncr();
	if (response_R(2) > 1) return -1;
	Ncc();
	send_cmd(cmd3);
	Ncr();
	if (response_R(6) > 1) return -1;
	RCA[0] = response_buffer[1];
	RCA[1] = response_buffer[2];
	Ncc();
	uint8_t cmd9_buf[5] = {cmd9[0], RCA[0], RCA[1], cmd9[3], cmd9[4]};
	send_cmd(cmd9_buf);
	Ncr();
	if (response_R(2) > 1) return -1;
	Ncc();
	uint8_t cmd7_buf[5] = {cmd7[0], RCA[0], RCA[1], cmd7[3], cmd7[4]};
	send_cmd(cmd7_buf);
	Ncr();
	if (response_R(1) > 1) return -1;
	Ncc();
	send_cmd(cmd16);
	Ncr();
	if (response_R(1) > 1) return -1;
	sd_card_ready = true;
	return 0;
}

int SD_read_lba(uint8_t *buf, uint32_t lba) {
	assert(sd_card_ready); // Must call init first
	Ncc();
	uint8_t cmd_buf[5] = {
		cmd17[0],
		(lba >> 15) & 0xff,
		(lba >> 7) & 0xff,
		(lba << 1) & 0xff,
		0,
	};
	send_cmd(cmd_buf);
	Ncr();
	while (1) {
		SD_CLK_LOW;
		SD_CLK_HIGH;
		if (!(SD_TEST_DAT)) {
			break;
		}
	}
	for (int i = 0; i < 512; i++) {
		uint8_t c = 0;
		for (int j = 0; j < 8; j++) {
			SD_CLK_LOW;
			SD_CLK_HIGH;
			c <<= 1;
			if (SD_TEST_DAT) {
				c |= 0x01;
			}
		}
		*buf = c;
		buf++;
	}
	for (int i = 0; i < 16; i++) {
		SD_CLK_LOW;
		SD_CLK_HIGH;
	}
	return 0;
}
