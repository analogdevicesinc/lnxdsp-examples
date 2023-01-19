/*
 **
 ** Source file generated on January 10, 2023 at 08:58:06.
 **
 ** Copyright (C) 2023 Analog Devices Inc., All Rights Reserved.
 **
 ** This file is generated when the FreeRTOS Add-In is added to a CrossCore Embedded Studio project,
 ** or if this file has been removed and the project is re-opened.
 **
 ** It is safe to edit this file.
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* User application includes. */
#include <sys/platform.h>
#include <sys/adi_core.h>
#include <string.h>
#include "rpmsg_config.h"
#include <rpmsg_platform.h>
#include <rpmsg_lite.h>
#include <rpmsg_ns.h>
#include <rpmsg-led-toggle-task.h>
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
volatile struct adi_resource_table *adi_resource_table_arm;
volatile struct sharc_resource_table *resource_table_arm;
volatile struct adi_resource_table *adi_resource_table_sharc;
volatile struct sharc_resource_table *resource_table_sharc;

#pragma section("seg_sdram_bsz_data", ZERO_INIT)
volatile uint8_t vring_buffer[ADI_VRING_BUFFER_SIZE];

/* Static variables for rpmsg-lite */
struct rpmsg_lite_instance rpmsg_SHARC_channel;
struct rpmsg_lite_ept_static_context sharc_sync_endpoint_context;
struct rpmsg_lite_instance rpmsg_ARM_channel;
struct rpmsg_lite_ept_static_context arm_sync_endpoint_context;

#define SHARC0_EP_ADDR0 154
#define SHARC0_EP_ADDR1 155
#define SHARC1_EP_ADDR 156


int32_t sharc_sync_call_back(void *payload, uint32_t payload_len, uint32_t src, void *priv) {
	return RL_RELEASE;
}

int rpmsg_SHARC_ping(char *msg, int len){
	uint32_t remote_addr;
	remote_addr = SHARC1_EP_ADDR;

	return rpmsg_lite_send(
			&rpmsg_SHARC_channel,
			&sharc_sync_endpoint_context.ept,
			remote_addr,
			msg, len, 0);
}

int32_t arm_sync_call_back(void *payload, uint32_t payload_len, uint32_t src, void *priv) {
	char msg[] = "toggle";
	rpmsg_SHARC_ping(msg, 6);
	return RL_RELEASE;
}

/*
 * Helper struct which represents memory ranges used by a vring.
 */
struct _mem_range{
	uint32_t start;
	uint32_t end;
};

/*
 * Helper function which reads memory ranges used by a vring.
 */
void vring_get_descriptor_range(volatile struct fw_rsc_vdev_vring *vring, struct _mem_range *range){
	struct vring_desc *desc = (struct vring_desc *)vring->da;
	range->start = (uint32_t)desc;
	range->end = (uint32_t)desc + vring_size(vring->num, vring->align);
}
void vring_get_buffer_range(volatile struct fw_rsc_vdev_vring *vring, struct _mem_range *range){
	struct vring_desc *desc = (struct vring_desc *)vring->da;
	uint32_t num = 2 * vring->num; // vring0 descriptor has pointer to buffers for both vrings
	range->start = (uint32_t)desc->addr;
	range->end = (uint32_t)desc->addr + num * (RL_BUFFER_PAYLOAD_SIZE +16);
}

int rsc_tbl_ready(void) {
	/* 0x1 acknowledge, 0x2 driver found, 0x4 driver ready*/
	return resource_table_arm->rpmsg_vdev.status == 0x7;
}

int sharc_ready(void) {
	return adi_resource_table_sharc->initialized == ADI_RESOURCE_TABLE_INIT_MAGIC;
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
						adi_cache_rr5,
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

	addr = SHARC0_EP_ADDR1;

	adi_resource_table_sharc = (struct adi_resource_table *)
		((uint32_t)&___MCAPI_common_start + ADI_RESOURCE_TABLE_SHARC1_OFFSET);
	memcpy((void*)adi_resource_table_sharc, (const void*)&rsc_tbl_local, sizeof(struct adi_resource_table));

	resource_table_sharc = &adi_resource_table_sharc->tbl;
	resource_table_sharc->vring[0].da = (uint32_t) rpmsg_instance->rvq->vq_ring_mem;
	resource_table_sharc->vring[1].da = (uint32_t) rpmsg_instance->tvq->vq_ring_mem;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC_channel,
			addr,
			&sharc_sync_call_back,
			NULL,
			&sharc_sync_endpoint_context);

	if (rpmsg_ept == RL_NULL) {
		return -1;
	}

	resource_table_sharc->rpmsg_vdev.status = 7; //Signal other core that resource table has been initialised

	while (!sharc_ready()) {
		/* Wait for sharc to complete init */
	}

	return 0;
}

/*
 * Initialize rpmsg channel to ARM core - remote
 */
int rpmsg_init_channel_to_ARM(void){
	uint32_t addr;
	int ret;

	struct rpmsg_lite_instance *rpmsg_instance;
	struct rpmsg_lite_endpoint *rpmsg_ept;
	adiCacheStatus status;
	struct _mem_range range0;
	struct _mem_range range1;

	adi_resource_table_arm = (struct adi_resource_table *)
		((uint32_t)&___MCAPI_common_start);
	resource_table_arm = &adi_resource_table_arm->tbl;

	while(!rsc_tbl_ready()) {
		/* Wait for resource table to be initialized */
	}

	// Get memory range which needs disabled cache
	// Read vring descriptors memory range
	vring_get_descriptor_range(&resource_table_arm->vring[0], &range0);
	vring_get_descriptor_range(&resource_table_arm->vring[1], &range1);
	range0.start = min(range0.start, range1.start);
	range0.end = max(range0.end, range1.end);
	// Disable cache for the descriptors memory range
	status = adi_cache_set_range ((void *)range0.start,
						(void *)(range0.end),
						adi_cache_rr6,
						adi_cache_noncacheable_range);

	// Read vring buffer memory range
	// vring1 has its own descriptors but share buffers with vring0
	vring_get_buffer_range(&resource_table_arm->vring[0], &range1);
	// Disable cache for the vring buffer range
	status = adi_cache_set_range ((void *)range1.start,
						(void *)(range1.end),
						adi_cache_rr7,
						adi_cache_noncacheable_range);

	rpmsg_instance = rpmsg_lite_remote_init(
			(void*)&resource_table_arm->rpmsg_vdev,
			RL_PLATFORM_SHARC_ARM_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_ARM_channel);

	if (rpmsg_instance == RL_NULL) {
		return -1;
	}

	addr = SHARC0_EP_ADDR0;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_ARM_channel,
			addr,
			&arm_sync_call_back,
			NULL,
			&arm_sync_endpoint_context);

	if (rpmsg_ept == RL_NULL) {
		return -1;
	}

	adi_resource_table_arm->initialized = ADI_RESOURCE_TABLE_INIT_MAGIC;

	return 0;
}

/*
 * User startup task. This task is created in main() and is
 * called when the scheduler starts.
 */
void rpmsgLedToggleTask( void *pvParameters )
{
	/*
	 * Add user defined code for your first task here.
	 * This function should never return.
	 * When complete it should call vTaskDelete().
	 */
	for ( ; ; )
	{
		int32_t Result=0, loop=1;
		char msg[] = "toggle";

		/* Begin adding your custom code here */
		Result = rpmsg_init_channel_to_SHARC();
		Result = rpmsg_init_channel_to_ARM();

		while(loop) {
			platform_time_delay(1000);
		}
	}
}
