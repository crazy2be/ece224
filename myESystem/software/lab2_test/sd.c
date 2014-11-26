#include "sd.h"

#include "altera_avalon_pio_regs.h"
#include <stdlib.h>
#include "sys/alt_irq.h"

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
static void Ncr(void);
static void Ncc(void);
static uint8_t response_R(uint8_t);
static uint8_t send_cmd(uint8_t *);

//-------------------------------------------------------------------------
// response_buffer, stores system registers after response_R(param) is used
// NB: this is implicitly initialized to all zeros
uint8_t response_buffer[20];

//-------------------------------------------------------------------------
//SD Card Commands

/*CMD0 *Resets all cards to Idle State.*/
const uint8_t cmd0[5] = { 0x40, 0x00, 0x00, 0x00, 0x00 };
/*CMD55 Indicates to the card that the next command is an
 application specific command rather than a
 standard command*/
const uint8_t cmd55[5] = { 0x77, 0x00, 0x00, 0x00, 0x00 };
/*CMD2 Asks any card to send their CID numbers on the CMD line. (Any card*/
/* that is connected to the host will respond.)*/
const uint8_t cmd2[5] = { 0x42, 0x00, 0x00, 0x00, 0x00 };
/*CMD3 Asks the card to publish a new relative address (RCA).*/
const uint8_t cmd3[5] = { 0x43, 0x00, 0x00, 0x00, 0x00 };
/*CMD7 Command toggles a card between the Stand-by and Transfer states*/
/*or between the Programming and Disconnect state.*/
const uint8_t cmd7[5] = { 0x47, 0x00, 0x00, 0x00, 0x00 };
/*CMD9 Addressed card sends its card-specific data (CSD) on the CMD line.*/
const uint8_t cmd9[5] = { 0x49, 0x00, 0x00, 0x00, 0x00 };
/*CMD16 Selects a block length (in bytes) for all following
 block commands (read and write).*/
const uint8_t cmd16[5] = { 0x50, 0x00, 0x00, 0x02, 0x00 };
/*CMD17 Reads a block of the size selected by the
 SET_BLOCKLEN command.*/
const uint8_t cmd17[5] = { 0x51, 0x00, 0x00, 0x00, 0x00 };
/*ACMD6 Defines the data bus width (’00’=1bit or ’10’=4 bits bus)
 to be used for data transfer.*/
const uint8_t acmd6[5] = { 0x46, 0x00, 0x00, 0x00, 0x02 };
/*ACMD41 Asks the accessed card to send its operating condition
 register (OCR) con tent in the response on the CMD
 line.*/
const uint8_t acmd41[5] = { 0x69, 0x0f, 0xf0, 0x00, 0x00 };
/*ACMD51 Reads the SD Configuration Register (SCR).*/
const uint8_t acmd51[5] = { 0x73, 0x00, 0x00, 0x00, 0x00 };

static void Ncr(void) {
	SD_CMD_IN;
	SD_CLK_LOW;
	SD_CLK_HIGH;
	SD_CLK_LOW;
	SD_CLK_HIGH;
}

static void Ncc(void) {
	int i;
	for (i = 0; i < 8; i++) {
		SD_CLK_LOW;
		SD_CLK_HIGH;
	}
}

uint8_t SD_card_init(void) {
	uint8_t x, y;
    uint8_t cmd_buffer[5];
    uint8_t RCA[2];

	SD_CMD_OUT;
	SD_DAT_IN;
	SD_CLK_HIGH;
	SD_CMD_HIGH;
	SD_DAT_LOW;
	for (x = 0; x < 40; x++)
		Ncr();
	y = send_cmd(cmd0);
	do {
		for (x = 0; x < 40; x++) {}
		Ncc();
		y = send_cmd(cmd55);
		Ncr();
		if (response_R(1) > 1) //response too long or crc error
			return 1;
		Ncc();
		y = send_cmd(acmd41);
		Ncr();
	} while (response_R(3) == 1);
	Ncc();
	y = send_cmd(cmd2);
	Ncr();
	if (response_R(2) > 1)
		return 1;
	Ncc();
	y = send_cmd(cmd3);
	Ncr();
	if (response_R(6) > 1)
		return 1;
	RCA[0] = response_buffer[1];
	RCA[1] = response_buffer[2];
	Ncc();

    memcpy(cmd_buffer, cmd9, sizeof(cmd_buffer));
	cmd_buffer[1] = RCA[0];
	cmd_buffer[2] = RCA[1];
	y = send_cmd(cmd_buffer);
	Ncr();
	if (response_R(2) > 1)
		return 1;
	Ncc();

    memcpy(cmd_buffer, cmd7, sizeof(cmd_buffer));
	cmd_buffer[1] = RCA[0];
	cmd_buffer[2] = RCA[1];
	y = send_cmd(cmd_buffer);
	Ncr();
	if (response_R(1) > 1)
		return 1;
	Ncc();
	y = send_cmd(cmd16);
	Ncr();
	if (response_R(1) > 1)
		return 1;
	return 0;
}

uint8_t SD_read_lba(uint8_t *buff, uint32_t lba, uint32_t seccnt) {
	Ncc();
	uint8_t cmd_buffer[5] = {cmd17[0], (lba >> 15) & 0xff,
			(lba >> 7) & 0xff, (lba << 1) & 0xff, 0};
	send_cmd(cmd_buffer);
	Ncr();

	while (1) {
		SD_CLK_LOW;
		SD_CLK_HIGH;
		if (!(SD_TEST_DAT))
			break;
	}
	//uint8_t c = 0;
	for (int i = 0; i < 512; i++) {
		uint8_t c = 0;
		for (int j = 0; j < 8; j++) {
			SD_CLK_LOW;
			SD_CLK_HIGH;
			c <<= 1;
			if (SD_TEST_DAT)
				c |= 0x01;
		}
		*buff = c;
		buff++;
	}
	for (int i = 0; i < 16; i++) {
		SD_CLK_LOW;
		SD_CLK_HIGH;
	}
	return 0;
}

static uint8_t response_R(uint8_t s) {
	uint8_t a = 0, b = 0, c = 0, r = 0, crc = 0;
	uint8_t i, j = 6, k;
	while (1) {
		SD_CLK_LOW;
		SD_CLK_HIGH;
		if (!(SD_TEST_CMD))
			break;
		if (crc++ > 100)
			return 2;
	}
	crc = 0;
	if (s == 2)
		j = 17;

	for (k = 0; k < j; k++) {
		c = 0;
		if (k > 0) //for crc culcar
			b = response_buffer[k - 1];
		for (i = 0; i < 8; i++) {
			SD_CLK_LOW;
			if (a > 0)
				c <<= 1;
			else
				i++;
			a++;
			SD_CLK_HIGH;
			if (SD_TEST_CMD)
				c |= 0x01;
			if (k > 0) {
				crc <<= 1;
				if ((crc ^ b) & 0x80)
					crc ^= 0x09;
				b <<= 1;
				crc &= 0x7f;
			}
		}
		if (s == 3) {
			if (k == 1 && (!(c & 0x80)))
				r = 1;
		}
		response_buffer[k] = c;
	}
	if (s == 1 || s == 6) {
		if (c != ((crc << 1) + 1))
			r = 2;
	}
	return r;
}

static uint8_t send_cmd(uint8_t *in) {
	int i, j;
	uint8_t b, crc = 0;
	SD_CMD_OUT;
	for (i = 0; i < 5; i++) {
		b = in[i];
		for (j = 0; j < 8; j++) {
			SD_CLK_LOW;
			if (b & 0x80)
				SD_CMD_HIGH;
			else
				SD_CMD_LOW;
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
	for (j = 0; j < 8; j++) {
		SD_CLK_LOW;
		if (crc & 0x80)
			SD_CMD_HIGH;
		else
			SD_CMD_LOW;
		SD_CLK_HIGH;
		crc <<= 1;
	}
	return b;
}
