/*
 * S25FL164.h
 *
 *  Created on: 7 may 2024
 *      Author: JVillanueva
 */

#ifndef S25FL164_H_
#define S25FL164_H_

#include <stdint.h>
#include "fsl_dspi.h"
#include "fsl_clock.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

#define MEM_CTAR      kDSPI_Ctar1
#define MEM_BAUDRATE  5000000U
#define MEM_CPOL      kDSPI_ClockPolarityActiveLow
#define MEM_CPHA      kDSPI_ClockPhaseSecondEdge


#define READ_COMMAND      0x03


void SPI_memory_init(void);

uint8_t SPI_memory_read_byte(uint32_t address);

void SPI_memory_read_segment(uint8_t * rx_buffer, uint32_t address, uint32_t size);

void SPI_memory_write_segment(uint16_t * tx_buffer, uint32_t address, uint32_t size);

void SPI_memory_erease_segment(uint32_t address, uint32_t lenght);

uint8_t SPI_memory_busy(void);


#endif /* S25FL164_H_ */
