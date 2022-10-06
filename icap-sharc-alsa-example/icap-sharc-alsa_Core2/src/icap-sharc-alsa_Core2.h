/*****************************************************************************
 * icap-sharc-alsa_Core2.h
 *****************************************************************************/

#ifndef __ICAP_SHARC_ALSA_CORE2_H__
#define __ICAP_SHARC_ALSA_CORE2_H__

#include <stdint.h>

#pragma pack(4)
struct sharc_sync_msg {
	uint32_t dma1_addr;
	uint32_t dma1_size;
	uint32_t dma2_addr;
	uint32_t dma2_size;
};
#pragma pack()

extern struct sharc_sync_msg dma_buffers;
extern volatile uint8_t dma_buf_initialized;

void init_rsc_tbl(void);
int rsc_tbl_ready(void);
int rpmsg_init_channel_to_ARM(void);
int rpmsg_init_channel_to_SHARC(void);

#endif /* __ICAP_SHARC_ALSA_CORE2_H__ */
