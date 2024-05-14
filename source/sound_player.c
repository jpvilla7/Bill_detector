/*
 * sound_player.c
 *
 *  Created on: 7 may 2024
 *      Author: JVillanueva
 */


#include "sound_player.h"
#include "S25FL164.h"
#include "fsl_dac.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_pit.h"
#include "NVIC.h"



uint8_t global_array[GLOBAL_LEN];

uint8_t reproduced_sound = false;
AT_QUICKACCESS_SECTION_DATA_ALIGN(edma_tcd_t tcdMemoryPoolPtr[1], 32);
edma_handle_t EDMA_Handle;

void DMA_Callback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
    	reproduced_sound = true;
    	PIT_StopTimer(PIT, kPIT_Chnl_0);
    }
}

bool detection_finished(void)
{
	return reproduced_sound;
}

void Init_peri(void)
{
	pit_config_t pitConfig;
	dac_config_t dacConfigStruct;
	edma_config_t userConfig;
	//DAC
	DAC_GetDefaultConfig(&dacConfigStruct);
	DAC_Init(DAC0, &dacConfigStruct);
	DAC_Enable(DAC0, true);

	//DMA
    DMAMUX_Init(DMAMUX0);
    DMAMUX_SetSource(DMAMUX0, 0, 58);
    DMAMUX_EnablePeriodTrigger(DMAMUX0, 0);
    DMAMUX_EnableChannel(DMAMUX0, 0);
    EDMA_GetDefaultConfig(&userConfig);
    EDMA_Init(DMA0, &userConfig);
    EDMA_CreateHandle(&EDMA_Handle, DMA0, 0);
    EDMA_SetCallback(&EDMA_Handle, DMA_Callback, NULL);
    EDMA_ResetChannel(EDMA_Handle.base, EDMA_Handle.channel);
	EDMA_TcdEnableInterrupts(tcdMemoryPoolPtr, kEDMA_MajorInterruptEnable);
	EDMA_TcdEnableAutoStopRequest(tcdMemoryPoolPtr, false);
    EDMA_EnableChannelRequest(DMA0, 0);

	NVIC_enable_interrupt_and_priotity(DMA_CH0_IRQ, PRIORITY_3);
	NVIC_global_enable_interrupts;


	PIT_GetDefaultConfig(&pitConfig);
	PIT_Init(PIT, &pitConfig);
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(95, CLOCK_GetFreq(kCLOCK_BusClk)));
}

void load_sound(bill_t bill_value)
{
	uint32_t address = 0;

	switch (bill_value)
	{
		case twenty:
			address = TWENTY_ADDR;
		break;

		case fifty:
			address = FIFTY_ADDR;
		break;

		case hundred:
			address = HUNDRED_ADDR;
		break;

		case two_hundred:
			address = TWO_HUN_ADDR;
		break;

		case five_hundred:
			address = FIVE_HUN_ADDR;
		break;
	}
	SPI_memory_read_segment(global_array, address, GLOBAL_LEN);
}

void play_sound(void)
{
	edma_transfer_config_t transferConfig;

    EDMA_PrepareTransferConfig(&transferConfig,
	    		   global_array,
				   2,
				   2,
				   DAC_ADDR,
				   2,
				   0,
				   2,
				   GLOBAL_LEN);
	EDMA_TcdSetMajorOffsetConfig(
			    tcdMemoryPoolPtr,
	   		    -GLOBAL_LEN,
			    0);

	EDMA_TcdSetTransferConfig(tcdMemoryPoolPtr, &transferConfig, NULL);
	EDMA_InstallTCD(DMA0, 0, tcdMemoryPoolPtr);

	PIT_StartTimer(PIT, kPIT_Chnl_0);
}

