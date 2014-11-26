#include <io.h>
#include <stdio.h>
#include "system.h"
#include "Open_I2C.h"

uint32_t I2C_Read_Period() {
	uint16_t low = IORD(OPEN_I2C_0_BASE, I2C_PRER_LO);
	uint16_t high = IORD(OPEN_I2C_0_BASE, I2C_PRER_HI);
	return (high << 16) + low;
}
uint8_t I2C_Read_Ctrl() {
	return IORD(OPEN_I2C_0_BASE, I2C_CTR);
}
uint8_t I2C_Read_RX() {
	return IORD(OPEN_I2C_0_BASE, I2C_RXR);
}
uint8_t I2C_Read_Status() {
	return IORD(OPEN_I2C_0_BASE, I2C_SR);
}
void I2C_Write_Period(uint32_t period) {
	IOWR(OPEN_I2C_0_BASE, I2C_PRER_LO, period&0xFF);
	IOWR(OPEN_I2C_0_BASE, I2C_PRER_HI, period>>16);
}
void I2C_Write_Ctrl(uint8_t value) {
	IOWR(OPEN_I2C_0_BASE, I2C_CTR, value);
}
void I2C_Write_TX(uint8_t value) {
	IOWR(OPEN_I2C_0_BASE, I2C_TXR, value);
}
void I2C_Write_CMD(uint8_t value) {
	IOWR(OPEN_I2C_0_BASE, I2C_CR, value);
}
uint8_t I2C_Read_CMD() {
	return IORD(OPEN_I2C_0_BASE, I2C_CR);
}
void I2C_Init(uint32_t period) {
	I2C_Ctrl_Reg a;
	I2C_Write_Period(period);
	a.I2C_Ctrl_Flags.CORE_ENABLE = 1;
	a.I2C_Ctrl_Flags.INT_ENABLE = 1;
	a.I2C_Ctrl_Flags.RESERVED = 0;
	I2C_Write_Ctrl(a.Value);
}
bool I2C_Send(uint8_t value, bool STA, bool STO) {
	I2C_CMD_Reg I2C_CMD;
	I2C_Status_Reg I2C_Status;
	//IOWR(LEDR_PIO_BASE, 0, 7); //THREE LEDS
	I2C_Write_TX(value);
	//IOWR(LEDR_PIO_BASE, 0, 15); //FOUR LEDS
	I2C_CMD.Value = 0;
	I2C_CMD.I2C_CMD_Flags.STA = (STA != 0);
	I2C_CMD.I2C_CMD_Flags.STO = (STO != 0);
	I2C_CMD.I2C_CMD_Flags.WR = 1;
	I2C_Write_CMD(I2C_CMD.Value);

	do {
		I2C_Status.Value = I2C_Read_Status();
	} while (I2C_Status.I2C_Status_Flags.TIP);
	//IOWR(LEDR_PIO_BASE, 0, 63); //SIX LEDS
	return !I2C_Status.I2C_Status_Flags.RXACK;
}

