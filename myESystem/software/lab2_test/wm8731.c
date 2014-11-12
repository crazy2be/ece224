#include <io.h>
#include <stdio.h>
#include "Open_I2C.h"
#include "wm8731.h"

void init_audio_codec()
{
	I2C_Init(10000000/I2C_FREQ-1);

	//  Check Audio CODEC on I2C Bus, Address = 0x34
	if (!I2C_Send(0x34,1,0)) {
		printf("\nCan't Find Audio CODEC on I2C Bus.\n");
		return;
	}
	printf("\nFind Audio CODEC on I2C Bus, Address = 0x34.\n");
	// Reset Device
	// I2C_Send(0xFE, 0, 0); //Reset MSB
	// I2C_Send(0x00, 0, 1); //Write 0 to Reset LSB

	I2C_Send(0x08, 0, 0); //Analog Audio Path Control MSB
	I2C_Send(0x12, 0, 1); //Analog Audio Path Control LSB
	I2C_Send(0x34, 1, 0);
	I2C_Send(0x0A, 0, 0); //Digital Audio Path Control MSB
	I2C_Send(0x00, 0, 1); //Digital Audio Path Control LSB
	I2C_Send(0x34, 1, 0);
	I2C_Send(0x0C, 0, 0); //Power Down Control MSB
	I2C_Send(0x00, 0, 1); //Power Down Control LSB
	I2C_Send(0x34, 1, 0);
	I2C_Send(0x0E, 0, 0); //Digital Audio Interface Format MSB
	I2C_Send(0x10, 0, 1); //Digital Audio Interface Format LSB
	I2C_Send(0x34, 1, 0);
	I2C_Send(0x10, 0, 0); //Sampling Control Register MSB
	I2C_Send(0x22, 0, 1); //Sampling Control Register LSB
	I2C_Send(0x34, 1, 0);
	I2C_Send(0x12, 0, 0); //Active Control Register MSB
	I2C_Send(0x01, 0, 1); //Active Control Register LSB
}
