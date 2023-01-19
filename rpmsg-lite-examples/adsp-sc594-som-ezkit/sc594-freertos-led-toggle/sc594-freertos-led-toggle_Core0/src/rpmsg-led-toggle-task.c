/*
 **
 ** Source file generated on January 10, 2023 at 08:57:18.
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
#define ADI_VRING_BUFFER_SIZE 0x00100000

/*
 * Expected resource table layout in the shared memory.
 * Initialised by ARM.
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
extern struct adi_resource_table __MCAPI_common_start;
volatile struct adi_resource_table *adi_resource_table;
volatile struct sharc_resource_table *resource_table;

__attribute__ ((section (".l3_data_uncached")))
uint8_t vring_buffer[ADI_VRING_BUFFER_SIZE];

/* Static variables for rpmsg-lite */
struct rpmsg_lite_instance rpmsg_SHARC_channel;
struct rpmsg_lite_ept_static_context sharc_sync_endpoint_context;
#define ARM_EP_ADDR 153
#define SHARC0_EP_ADDR 154

int32_t sharc_sync_call_back(void *payload, uint32_t payload_len, uint32_t src, void *priv) {
	return RL_RELEASE;
}

int sharc_ready(void) {
	return adi_resource_table->initialized == ADI_RESOURCE_TABLE_INIT_MAGIC;
}


/*
 * Initialize rpmsg channel to SHARC core - master
 */
int rpmsg_init_channel_to_SHARC0(void) {
	volatile struct rpmsg_lite_instance *rpmsg_instance;
	volatile struct rpmsg_lite_endpoint *rpmsg_ept;
	uint32_t addr;

	rpmsg_instance = rpmsg_lite_master_init(
			(void*)vring_buffer,
			ADI_VRING_BUFFER_SIZE,
			RL_PLATFORM_ARM_SHARC0_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_SHARC_channel);

	if (rpmsg_instance == RL_NULL) {
		return -1;
	}

	addr = ARM_EP_ADDR;

	adi_resource_table = (struct adi_resource_table *)
		((uint32_t)&__MCAPI_common_start);
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

	while (!sharc_ready()) {
		/* Wait for sharc to complete init */
	}

	return 0;
}

int rpmsg_SHARC0_ping(char *msg, int len) {
	uint32_t remote_addr;
	remote_addr = SHARC0_EP_ADDR;

	return rpmsg_lite_send(
			&rpmsg_SHARC_channel,
			&sharc_sync_endpoint_context.ept,
			remote_addr,
			msg, len, 0);
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
		int32_t loop=1;
		char msg[] = "toggle";

		if (rpmsg_init_channel_to_SHARC0() == 0) {
			while(loop) {
				platform_time_delay(500);
				rpmsg_SHARC0_ping(msg, 6);
			}
		}
	}
}
