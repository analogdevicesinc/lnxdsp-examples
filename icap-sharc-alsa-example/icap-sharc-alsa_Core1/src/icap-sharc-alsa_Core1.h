/*********************************************************************************
Copyright(c) 2021 Analog Devices, Inc. All Rights Reserved.
This software is proprietary and confidential. By using this software you agree
to the terms of the associated Analog Devices License Agreement.
*********************************************************************************/

/*****************************************************************************
 * Audio_Talkthrough_I2S.h
 *****************************************************************************/

#ifndef __AUDIO_TALKTHROUGH_I2S_H__
#define __AUDIO_TALKTHROUGH_I2S_H__

/* Add your custom header content here */

#include <stdint.h>
#include <services/int/adi_int.h>
#include <drivers/sport/adi_sport.h>

#define SPORT_DEVICE_4A 			4u			/* SPORT device number */
#define SPORT_DEVICE_4B 			4u			/* SPORT device number */
#define TWIDEVNUM     				2u         /* TWI device number */

#define BITRATE       				(100u)      /* kHz */
#define DUTYCYCLE     				(50u)       /* percent */
#define PRESCALEVALUE 				(12u)       /* fSCLK/10MHz (112.5 sclk0_0) */
#define BUFFER_SIZE   				(8u)

#define TARGETADDR    				(0x38u)     /* hardware address */
#define TARGETADDR_1962    			(0x04u)     /* hardware address of adau1962 DAC */
#define TARGETADDR_1979    			(0x11u)     /* hardware address of adau1979 ADC */


#define SPORT_4A_SPU  					71
#define SPORT_4B_SPU   					72

#define DMA_NUM_DESC 				2u

#define SUCCESS   0
#define FAILED   -1

#define CHECK_RESULT(eResult) \
        if(eResult != 0)\
		{\
			return (1);\
        }

#define REPORT_ERROR        	 printf
#define DEBUG_INFORMATION        printf

#pragma pack(4)
struct sharc_sync_msg {
	uint32_t dma1_addr;
	uint32_t dma1_size;
	uint32_t dma2_addr;
	uint32_t dma2_size;
};
#pragma pack(0)

void init_rsc_tbl(void);
int rsc_tbl_ready(void);
int rpmsg_init_channel_to_ARM(void);
int rpmsg_init_channel_to_SHARC(void);
int rpmsg_SHARC_sync(struct sharc_sync_msg *msg);


#endif /* __AUDIO_TALKTHROUGH_I2S_H__ */
