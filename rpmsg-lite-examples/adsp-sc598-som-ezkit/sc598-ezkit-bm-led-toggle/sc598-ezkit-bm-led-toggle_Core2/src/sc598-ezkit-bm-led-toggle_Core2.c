/* SPDX-License-Identifier: Apache-2.0 */
/*
  Example toggling LED upon receiving RPMsg-lite message
  Copyright (c) 2021-2022, Analog Devices, Inc.  All rights reserved.
*/

#include <sys/platform.h>
#include <sys/adi_core.h>
#include "rpmsg_config.h"
#include <rpmsg_platform.h>
#include <rpmsg_lite.h>
#include <rpmsg_ns.h>
#include "adi_initialize.h"
#include "led_blink.h"
#include <sysreg.h>

#define ADI_RESOURCE_TABLE_INIT_MAGIC (0xADE0AD0E)
#define ADI_RESOURCE_TABLE_SHARC1_OFFSET (0x400) //1KiB

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
struct adi_resource_table{
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
		.tbl.rpmsg_vdev = {
			RSC_VDEV, 		/* virtio dev type */
			7, 				/* it's rpmsg virtio */
			1, 				/* kick sharc0 */
			/* 1<<0 is VIRTIO_RPMSG_F_NS bit defined in virtio_rpmsg_bus.c */
			1<<0, 0, 0, 0,	/* dfeatures, gfeatures, config len, status */
			2, 				/* num_of_vrings */
			{0, 0,}, 		/* reserved */
		},
		.tbl.vring = {
			{(uint32_t)-1, (uint32_t)-1, (uint32_t)-1, 1, 0}, /* da, align and num allocated by other SHARC */
			{(uint32_t)-1, (uint32_t)-1, (uint32_t)-1, 1, 0}, /* da, align and num allocated by other SHARC*/
		},
};

extern "asm" struct adi_resource_table ___MCAPI_common_start;
volatile struct adi_resource_table *adi_resource_table;
volatile struct sharc_resource_table *resource_table;

/* Static variables for rpmsg-lite */
struct rpmsg_lite_instance rpmsg_SHARC_channel;
struct rpmsg_lite_ept_static_context sharc_sync_endpoint_context;

#define SHARC_SYNC_EP_ADDR_CORE0 155
#define SHARC_SYNC_EP_ADDR_CORE1 156

int32_t sharc_sync_call_back(void *payload, uint32_t payload_len, uint32_t src, void *priv){
	uint32_t start, end, tmp;
	toggle_led(LED_COM);
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
	return resource_table->rpmsg_vdev.status == 0x7;
}

/*
 * Initialize rpmsg channel to SHARC core - remote
 */
int rpmsg_init_channel_to_SHARC(void){
	uint32_t addr;
	int ret;

	struct rpmsg_lite_instance *rpmsg_instance;
	struct rpmsg_lite_endpoint *rpmsg_ept;
	adiCacheStatus status;
	struct _mem_range range0;
	struct _mem_range range1;

	adi_resource_table = (struct adi_resource_table *)
		((uint32_t)&___MCAPI_common_start + ADI_RESOURCE_TABLE_SHARC1_OFFSET);
	resource_table = &adi_resource_table->tbl;

	while(!rsc_tbl_ready()){
		/* Wait for resource table to be initialized */
	}

	// Get memory range which needs disabled cache
	// Read vring descriptors memory range
	vring_get_descriptor_range(&resource_table->vring[0], &range0);
	vring_get_descriptor_range(&resource_table->vring[1], &range1);
	range0.start = min(range0.start, range1.start);
	range0.end = max(range0.end, range1.end);
	// Disable cache for the descriptors memory range
	status = adi_cache_set_range ((void *)range0.start,
						(void *)(range0.end),
						adi_cache_rr6,
						adi_cache_noncacheable_range);

	// Read vring buffer memory range
	// vring1 has its own descriptors but share buffers with vring0
	vring_get_buffer_range(&resource_table->vring[0], &range1);
	// Disable cache for the vring buffer range
	status = adi_cache_set_range ((void *)range1.start,
						(void *)(range1.end),
						adi_cache_rr7,
						adi_cache_noncacheable_range);

	rpmsg_instance = rpmsg_lite_remote_init(
			(void*)&resource_table->rpmsg_vdev,
			RL_PLATFORM_SHARC_SHARC_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_SHARC_channel);

	if (rpmsg_instance == RL_NULL) {
		return -1;
	}

	addr = SHARC_SYNC_EP_ADDR_CORE1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC_channel,
			addr,
			&sharc_sync_call_back,
			NULL,
			&sharc_sync_endpoint_context);

	if (rpmsg_ept == RL_NULL) {
		return -1;
	}

	adi_resource_table->initialized = ADI_RESOURCE_TABLE_INIT_MAGIC;

	return 0;
}

/* Disable prefetch buffers for SC598 as it causes issues with triggering other SHARC */
const uint32_t __prefetch_ctrl = 0;

int main(int argc, char *argv[])
{
	int32_t Result=0, loop=1;

	adi_initComponents();
	
	init_leds();
	test_leds();

	Result = rpmsg_init_channel_to_SHARC();
	if (Result) {
		turn_on_led(LED_ERROR);
	}

	while(loop) {
		platform_time_delay(1000);
	}

	return 0;
}
