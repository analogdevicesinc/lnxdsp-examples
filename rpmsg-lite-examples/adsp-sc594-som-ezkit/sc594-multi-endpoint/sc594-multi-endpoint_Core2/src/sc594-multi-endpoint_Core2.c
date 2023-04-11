/* SPDX-License-Identifier: Apache-2.0 */
/*
  sc594-multi-endpoint_Core2.c
  Copyright (c) 2023, Analog Devices, Inc.  All rights reserved.
*/

#include <sys/platform.h>
#include <sys/adi_core.h>
#include "rpmsg_config.h"
#include <rpmsg_platform.h>
#include <rpmsg_lite.h>
#include <rpmsg_ns.h>
#include "adi_initialize.h"
#include "led_blink.h"

#define ADI_RESOURCE_TABLE_INIT_MAGIC (0xADE0AD0E)
#define ADI_RESOURCE_TABLE_ARM_OFFSET (0x400)		//1KiB
#define ADI_RESOURCE_TABLE_SHARC0_OFFSET (0x800) 	//2KiB

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

extern "asm" struct adi_resource_table ___MCAPI_common_start;
volatile struct adi_resource_table *adi_resource_table_sharc;
volatile struct sharc_resource_table *resource_table_sharc;
volatile struct adi_resource_table *adi_resource_table_arm;
volatile struct sharc_resource_table *resource_table_arm;

/* Static variables for rpmsg-lite */
struct rpmsg_lite_instance rpmsg_ARM_channel;
struct rpmsg_lite_ept_static_context arm_ctrl_endpoint1_context;
struct rpmsg_lite_ept_static_context arm_ctrl_endpoint2_context;
struct rpmsg_lite_ept_static_context arm_tuning_endpoint1_context;
struct rpmsg_lite_ept_static_context arm_tuning_endpoint2_context;

struct rpmsg_lite_instance rpmsg_SHARC_channel;
struct rpmsg_lite_ept_static_context sharc_ctrl_endpoint_context;

#define SHARC1_CTRL_EP1_ADDR 170
#define SHARC1_CTRL_EP2_ADDR 171
#define SHARC1_CTRL_EP3_ADDR 172
#define SHARC1_TUNING_EP1_ADDR 175
#define SHARC1_TUNING_EP2_ADDR 176

int32_t arm_to_sharc1_ctrl1(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	toggle_led(LED);
	return RL_RELEASE;
}

int32_t arm_to_sharc1_ctrl2(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	toggle_led(LED9);
	return RL_RELEASE;
}

int32_t sharc0_to_sharc1_ctrl1(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	toggle_led(LED10);
	return RL_RELEASE;
}

int32_t arm_to_sharc1_tuning1(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	char ack[] = "ack";

	// Send ack back to ARM
	rpmsg_lite_send(
				&rpmsg_ARM_channel,
				&arm_tuning_endpoint1_context.ept,
				src,
				ack, sizeof(ack), 0);
	return RL_RELEASE;
}

int32_t arm_to_sharc1_tuning2(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	char ack[] = "ack";

	// Send ack back to ARM
	rpmsg_lite_send(
				&rpmsg_ARM_channel,
				&arm_tuning_endpoint2_context.ept,
				src,
				ack, sizeof(ack), 0);
	return RL_RELEASE;
}

/*
 * Helper struct which represents memory ranges used by a vring.
 */
struct _mem_range {
	uint32_t start;
	uint32_t end;
};

/*
 * Helper function which reads memory ranges used by a vring.
 */
void vring_get_descriptor_range(volatile struct fw_rsc_vdev_vring *vring, struct _mem_range *range)
{
	struct vring_desc *desc = (struct vring_desc *)vring->da;

	range->start = (uint32_t)desc;
	range->end = (uint32_t)desc + vring_size(vring->num, vring->align);
}

void vring_get_buffer_range(volatile struct fw_rsc_vdev_vring *vring, struct _mem_range *range)
{
	struct vring_desc *desc = (struct vring_desc *)vring->da;
	uint32_t num = 2 * vring->num; // vring0 descriptor has pointer to buffers for both vrings

	range->start = (uint32_t)desc->addr;
	range->end = (uint32_t)desc->addr + num * (RL_BUFFER_PAYLOAD_SIZE +16);
}

int rsc_tbl_ready(volatile struct sharc_resource_table * resource_table)
{
	/* 0x1 acknowledge, 0x2 driver found, 0x4 driver ready*/
	return resource_table->rpmsg_vdev.status == 0x7;
}

/*
 * Initialise rpmsg channel to SHARC core - remote
 */
int rpmsg_init_channel_to_SHARC(void)
{
	struct rpmsg_lite_instance *rpmsg_instance;
	struct rpmsg_lite_endpoint *rpmsg_ept;
	adiCacheStatus status;
	struct _mem_range range0;
	struct _mem_range range1;

	adi_resource_table_sharc = (struct adi_resource_table *)
		((uint32_t)&___MCAPI_common_start + ADI_RESOURCE_TABLE_SHARC0_OFFSET);
	resource_table_sharc = &adi_resource_table_sharc->tbl;

	while(!rsc_tbl_ready(resource_table_sharc)){
		/* Wait for resource table to be initialised */
	}

	// Read vring descriptors memory range
	vring_get_descriptor_range(&resource_table_sharc->vring[0], &range0);
	vring_get_descriptor_range(&resource_table_sharc->vring[1], &range1);
	range0.start = min(range0.start, range1.start);
	range0.end = max(range0.end, range1.end);

	// Disable cache for the descriptors memory range
	status = adi_cache_set_range ((void *)range0.start,
						(void *)(range0.end),
						adi_cache_rr6,
						adi_cache_noncacheable_range);

	// Read vring buffer memory range
	// vring1 has its own descriptors but share buffers with vring0
	vring_get_buffer_range(&resource_table_sharc->vring[0], &range1);
	// Disable cache for the vring buffer range
	status = adi_cache_set_range ((void *)range1.start,
						(void *)(range1.end),
						adi_cache_rr7,
						adi_cache_noncacheable_range);

	rpmsg_instance = rpmsg_lite_remote_init(
			(void*)&resource_table_sharc->rpmsg_vdev,
			RL_PLATFORM_SHARC_SHARC_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_SHARC_channel);

	if (rpmsg_instance == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC_channel,
			SHARC1_CTRL_EP3_ADDR,
			&sharc0_to_sharc1_ctrl1,
			NULL,
			&sharc_ctrl_endpoint_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	adi_resource_table_sharc->initialized = ADI_RESOURCE_TABLE_INIT_MAGIC;

	return 0;
}

/*
 * Initialise rpmsg channel to ARM core - remote
 */
int rpmsg_init_channel_to_ARM(void)
{
	int ret;

	struct rpmsg_lite_instance *rpmsg_instance;
	struct rpmsg_lite_endpoint *rpmsg_ept;
	adiCacheStatus status;
	struct _mem_range range0;
	struct _mem_range range1;

	adi_resource_table_arm = (struct adi_resource_table *)
		((uint32_t)&___MCAPI_common_start + ADI_RESOURCE_TABLE_ARM_OFFSET);
	resource_table_arm = &adi_resource_table_arm->tbl;

	while(!rsc_tbl_ready(resource_table_arm)) {
		/* Wait for resource table to be initialised */
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
						adi_cache_rr4,
						adi_cache_noncacheable_range);

	// Read vring buffer memory range
	// vring1 has its own descriptors but share buffers with vring0
	vring_get_buffer_range(&resource_table_arm->vring[0], &range1);
	// Disable cache for the vring buffer range
	status = adi_cache_set_range ((void *)range1.start,
						(void *)(range1.end),
						adi_cache_rr5,
						adi_cache_noncacheable_range);

	rpmsg_instance = rpmsg_lite_remote_init(
			(void*)&resource_table_arm->rpmsg_vdev,
			RL_PLATFORM_SHARC_ARM_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_ARM_channel);

	if (rpmsg_instance == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_ARM_channel,
			SHARC1_CTRL_EP1_ADDR,
			&arm_to_sharc1_ctrl1,
			NULL,
			&arm_ctrl_endpoint1_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_ARM_channel,
			SHARC1_CTRL_EP2_ADDR,
			&arm_to_sharc1_ctrl2,
			NULL,
			&arm_ctrl_endpoint2_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_ARM_channel,
			SHARC1_TUNING_EP1_ADDR,
			&arm_to_sharc1_tuning1,
			NULL,
			&arm_tuning_endpoint1_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_ARM_channel,
			SHARC1_TUNING_EP2_ADDR,
			&arm_to_sharc1_tuning2,
			NULL,
			&arm_tuning_endpoint2_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	adi_resource_table_arm->initialized = ADI_RESOURCE_TABLE_INIT_MAGIC;

	return 0;
}

int main(int argc, char *argv[])
{

	int32_t result=0, loop=1;

	adi_initComponents();
	
	init_leds();
	test_leds();

	result = rpmsg_init_channel_to_SHARC();
	if (result)
		return -1;

	result = rpmsg_init_channel_to_ARM();
	if (result)
		return -1;


	while(loop) {
		platform_time_delay(1000);
	}

	return 0;
}
