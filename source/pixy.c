/*
 * pixy.c
 *
 *  Created on: 8 may 2024
 *      Author: JVillanueva
 */


#include "pixy.h"

void pixy_mod_init(void)
{
	i2c_master_config_t masterConfig;
	uint32_t sourceClock = 0;

	CLOCK_EnableClock(kCLOCK_PortB);
    PORT_SetPinMux(PORTB,  2,   kPORT_MuxAlt2);
    PORT_SetPinMux(PORTB,  3,   kPORT_MuxAlt2);

    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = 100000;

    sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

    I2C_MasterInit(I2C0, &masterConfig, sourceClock);
}

bill_t get_bill(void){
	bill_t bill;
	uint8_t data[20];
	uint8_t command[6];
	command[0]=0xae;
	command[1]=0xc1;
	command[2]=32;
	command[3]=2;
	command[4]=255;
	command[5]=255;

	 i2c_master_transfer_t masterXfer;

	 masterXfer.slaveAddress   = 0x54;
	 masterXfer.direction      = kI2C_Write;
	 masterXfer.subaddress     = 0;
	 masterXfer.subaddressSize = 0;
	 masterXfer.data           = command;
	 masterXfer.dataSize       = 6;
	 masterXfer.flags          = kI2C_TransferDefaultFlag;
	 I2C_MasterTransferBlocking(I2C0, &masterXfer);

	 masterXfer.slaveAddress   = 0x54;
	 masterXfer.direction      = kI2C_Read;
	 masterXfer.subaddress     = 0;
	 masterXfer.subaddressSize = 0;
	 masterXfer.data           = data;
	 masterXfer.dataSize       = 20;
	 masterXfer.flags          = kI2C_TransferDefaultFlag;
	 I2C_MasterTransferBlocking(I2C0, &masterXfer);

	 bill = data[6];

	 return bill;
}
