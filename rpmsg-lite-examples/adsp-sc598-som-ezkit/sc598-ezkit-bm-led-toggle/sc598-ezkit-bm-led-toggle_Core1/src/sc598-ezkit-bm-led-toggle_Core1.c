/* SPDX-License-Identifier: Apache-2.0 */
/*
  Example toggling LED upon receiving RPMsg-lite message
  Copyright (c) 2021-2022, Analog Devices, Inc.  All rights reserved.
*/

#include <sys/platform.h>
#include <sys/adi_core.h>
#include <string.h>
#include "rpmsg_config.h"
#include <rpmsg_platform.h>
#include <rpmsg_lite.h>
#include <rpmsg_ns.h>
#include "adi_initialize.h"

#define ADI_RESOURCE_TABLE_INIT_MAGIC (0xADE0AD0E)
#define ADI_RESOURCE_TABLE_SHARC1_OFFSET (0x400) //1KiB
#define ADI_VRING_BUFFER_SIZE 0x00100000

/*
 * Expected resource table layout in the shared memory.
 * Initialised by SHARC0.
 */
RL_PACKED_BEGIN
struct sharc_resource_table {
	struct resource_table table_hdr;
	unsigned int offset[1];
	struct fw_rsc_vdev rpmsg_vdev;
	struct fw_rsc_vdev_vring vring[2];
}RL_PACKED_END;

RL_PACKED_BEGIN
struct adi_resource_table {
	uint8_t tag[16];
	uint32_t version;
	uint32_t initialized;
	uint32_t reserved[8];

	struct sharc_resource_table tbl;
}RL_PACKED_END;

const struct adi_resource_table rsc_tbl_local = {
		.tag = "AD-RESOURCE-TBL",
		.version = 1,
		.initialized = 0,
		.tbl.table_hdr = {
			/* resource table header */
			1, 			/* version */
			1, 			/* number of table entries */
			{0, 0,},	/* reserved fields */
		},
		.tbl.offset = {offsetof(struct sharc_resource_table, rpmsg_vdev),
		},
		.tbl.rpmsg_vdev = {RSC_VDEV, /* virtio dev type */
			7, 				/* it's rpmsg virtio */
			1, 				/* kick sharc0 */
			/* 1<<0 is VIRTIO_RPMSG_F_NS bit defined in virtio_rpmsg_bus.c */
			1<<0, 0, 0, 0,	/* dfeatures, gfeatures, config len, status */
			2, 				/* num_of_vrings */
			{0, 0,}, 		/* reserved */
		},
		.tbl.vring = {
			{(uint32_t)-1, VRING_ALIGN, RL_BUFFER_COUNT, 1, 0}, /* da allocated by rpmsg_lite_master_init */
			{(uint32_t)-1, VRING_ALIGN, RL_BUFFER_COUNT, 1, 0}, /* da allocated by rpmsg_lite_master_init */
		},
};

/*
 * Declare the resource table used for sharing shmem vring details.
 * The ___MCAPI_common_start address is defined in app.ldf
 */
extern "asm" struct adi_resource_table ___MCAPI_common_start;
volatile struct adi_resource_table *adi_resource_table;
volatile struct sharc_resource_table *resource_table;

#pragma section("seg_sdram_bsz_data", ZERO_INIT)
volatile uint8_t vring_buffer[ADI_VRING_BUFFER_SIZE];

/* Static variables for rpmsg-lite */
struct rpmsg_lite_instance rpmsg_SHARC_channel;
struct rpmsg_lite_ept_static_context sharc_sync_endpoint_context;
#define SHARC_SYNC_EP_ADDR_CORE0 155
#define SHARC_SYNC_EP_ADDR_CORE1 156

int32_t sharc_sync_call_back(void *payload, uint32_t payload_len, uint32_t src, void *priv){
	return RL_RELEASE;
}

/*
 * Initialize rpmsg channel to SHARC core - master
 */
int rpmsg_init_channel_to_SHARC(void) {
	volatile struct rpmsg_lite_instance *rpmsg_instance;
	volatile struct rpmsg_lite_endpoint *rpmsg_ept;
	adiCacheStatus status;
	uint32_t addr;

	// Disable cache for the descriptors memory range
	status = adi_cache_set_range ((void *)vring_buffer,
						(void *)(vring_buffer + ADI_VRING_BUFFER_SIZE),
						adi_cache_rr6,
						adi_cache_noncacheable_range);

	rpmsg_instance = rpmsg_lite_master_init(
			(void*)vring_buffer,
			ADI_VRING_BUFFER_SIZE,
			RL_PLATFORM_SHARC_SHARC_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_SHARC_channel);

	if (rpmsg_instance == RL_NULL) {
		return -1;
	}

	addr = SHARC_SYNC_EP_ADDR_CORE0;

	adi_resource_table = (struct adi_resource_table *)
		((uint32_t)&___MCAPI_common_start + ADI_RESOURCE_TABLE_SHARC1_OFFSET);
	memcpy((void*)adi_resource_table, (const void*)&rsc_tbl_local, sizeof(struct adi_resource_table));

	resource_table = &adi_resource_table->tbl;
	resource_table->vring[0].da = (uint32_t) rpmsg_instance->rvq->vq_ring_mem;
	resource_table->vring[1].da = (uint32_t) rpmsg_instance->tvq->vq_ring_mem;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC_channel,
			addr,
			&sharc_sync_call_back,
			NULL,
			&sharc_sync_endpoint_context);

	if (rpmsg_ept == RL_NULL) {
		return -1;
	}

	resource_table->rpmsg_vdev.status = 7; //Signal other core that resource table has been initialised

	return 0;
}

int rpmsg_SHARC_ping(char *msg, int len){
	uint32_t remote_addr;
	remote_addr = SHARC_SYNC_EP_ADDR_CORE1;

	return rpmsg_lite_send(
			&rpmsg_SHARC_channel,
			&sharc_sync_endpoint_context.ept,
			remote_addr,
			msg, len, 0);
}

/* Disable prefetch buffers for SC598 as it causes issues with triggering other SHARC */
const uint32_t __prefetch_ctrl = 0;

int main(int argc, char *argv[])
{
	int32_t Result=0, loop=1, counter = 0;
	char msg[] = "toggle";
	/**
	 * Initialize managed drivers and/or services that have been added to 
	 * the project.
	 * @return zero on success 
	 */
	adi_initComponents();
	


	/* Begin adding your custom code here */
	Result = rpmsg_init_channel_to_SHARC();

	while(loop) {
		platform_time_delay(500);
		rpmsg_SHARC_ping(msg, 6);
	}

	return 0;
}
