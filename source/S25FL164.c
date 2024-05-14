/*
 * S25FL164.c
 *
 *  Created on: 7 may 2024
 *      Author: JVillanueva
 */

#include "S25FL164.h"


static uint8_t page_array[256];

void SPI_memory_init(void)
{
	dspi_master_config_t masterConfig;
	uint32_t srcClock_Hz;

	CLOCK_SetSimSafeDivs();

	// SPI pins:
	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortD);

	PORT_SetPinMux(PORTC,  3,  kPORT_MuxAlt2);
	PORT_SetPinMux(PORTD, 1,    kPORT_MuxAlt2);
	PORT_SetPinMux(PORTD, 2,   kPORT_MuxAlt2);
	PORT_SetPinMux(PORTD, 3,   kPORT_MuxAlt2);

	// SPI config:
	masterConfig.whichCtar 								  = MEM_CTAR;
	masterConfig.ctarConfig.baudRate 					  = MEM_BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame 				  = 8U;
	masterConfig.ctarConfig.cpol 						  = MEM_CPOL;
	masterConfig.ctarConfig.cpha 						  = MEM_CPHA;
	masterConfig.ctarConfig.direction 					  = kDSPI_MsbFirst;

	masterConfig.ctarConfig.pcsToSckDelayInNanoSec 		  = 0;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec 	  = 0;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 0;

	masterConfig.whichPcs 			= kDSPI_Pcs1;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig.enableContinuousSCK 		= false;
	masterConfig.enableRxFifoOverWrite 		= false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint 				= kDSPI_SckToSin0Clock;

	srcClock_Hz = CLOCK_GetFreq(DSPI0_CLK_SRC);
	DSPI_MasterInit(SPI0, &masterConfig, srcClock_Hz);
}


uint8_t SPI_memory_read_byte(uint32_t address)
{
    dspi_half_duplex_transfer_t masterXfer;
    uint8_t command[] = {READ_COMMAND, (address >> 16), ((address & 0xFF00) >> 8),
                         (address & 0xFF)};
    uint8_t read_byte[1] = {0};

	masterXfer.txData      = command;
	masterXfer.rxData      = read_byte;
	masterXfer.txDataSize  = 4;
	masterXfer.rxDataSize  = 1;
	masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;
	masterXfer.isPcsAssertInTransfer = true;
	masterXfer.isTransmitFirst       = true;
	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);

	return read_byte[0];
}

void SPI_memory_read_segment(uint8_t * rx_buffer, uint32_t address, uint32_t size)
{
    dspi_half_duplex_transfer_t masterXfer;
    uint8_t command[] = {READ_COMMAND, (address >> 16), ((address & 0xFF00) >> 8),
                         (address & 0xFF)};

	masterXfer.txData      = command;
	masterXfer.rxData      = rx_buffer;
	masterXfer.txDataSize  = 4;
	masterXfer.rxDataSize  = size;
	masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;
	masterXfer.isPcsAssertInTransfer = true;
	masterXfer.isTransmitFirst       = true;

	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);
}

void SPI_memory_write_segment(uint16_t * tx_buffer, uint32_t address, uint32_t size)
{
    dspi_half_duplex_transfer_t masterXfer;
    uint8_t WE_cmnd = 0x6;
    uint8_t command[] = {0x2, (address >> 16), ((address & 0xFF00) >> 8),
                         (address & 0xFF)};

    SPI_memory_erease_segment(address, 0);


    masterXfer.txData      = &WE_cmnd;
    masterXfer.rxData      = NULL;
    masterXfer.txDataSize  = 1;
    masterXfer.rxDataSize  = 0U;
    masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;
    masterXfer.isPcsAssertInTransfer = false;
    masterXfer.isTransmitFirst       = true;
    DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);

    masterXfer.txData      = command;
    masterXfer.rxData      = NULL;
    masterXfer.txDataSize  = 4;
    masterXfer.rxDataSize  = 0U;
    masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;
    masterXfer.isPcsAssertInTransfer = true;
    masterXfer.isTransmitFirst       = true;

    DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);

    masterXfer.txData      = page_array;
    masterXfer.rxData      = NULL;
    masterXfer.txDataSize  = size;
    masterXfer.rxDataSize  = 0U;
    masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;
    masterXfer.isPcsAssertInTransfer = false;
    masterXfer.isTransmitFirst       = true;

    DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);

    while(SPI_memory_busy())
    {
    }

}

void SPI_memory_erease_segment(uint32_t address, uint32_t lenght)
{
	dspi_half_duplex_transfer_t masterXfer;
	uint8_t write_enble_cmnd = 0x6;
    uint8_t command[] = {0xD8, (address >> 16), ((address & 0xFF00) >> 8),
                         (address & 0xFF)};

	masterXfer.txData      = &write_enble_cmnd;
	masterXfer.rxData      = NULL;
	masterXfer.txDataSize  = 1;
	masterXfer.rxDataSize  = 0U;
	masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;
	masterXfer.isPcsAssertInTransfer = false;
	masterXfer.isTransmitFirst       = true;

	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);

	masterXfer.txData      = command;
	masterXfer.rxData      = NULL;
	masterXfer.txDataSize  = 4;
	masterXfer.rxDataSize  = 0U;
	masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;
	masterXfer.isPcsAssertInTransfer = false;
	masterXfer.isTransmitFirst       = true;

	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);

	while(SPI_memory_busy())
	{
	}
}

uint8_t SPI_memory_busy(void)
{
	dspi_half_duplex_transfer_t masterXfer;
	uint8_t SR1_cmmnd = 0x5;
	uint8_t value = 0;

	masterXfer.txData      = &SR1_cmmnd;
	masterXfer.rxData      = &value;
	masterXfer.txDataSize  = 1U;
	masterXfer.rxDataSize  = 1U;
	masterXfer.configFlags = kDSPI_MasterCtar1 | kDSPI_MasterPcs1 | kDSPI_MasterPcsContinuous;
	masterXfer.isPcsAssertInTransfer = true;
	masterXfer.isTransmitFirst       = true;
	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &masterXfer);


	if(value)
	{
		return 1;
	}
	return 0;
}

