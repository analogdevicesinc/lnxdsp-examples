/* SPDX-License-Identifier: Apache-2.0 */
/*
  sc594-multi-endpoint_Core0.c
  Copyright (c) 2021-2023, Analog Devices, Inc.  All rights reserved.
*/

#include <sys/platform.h>
#include <sys/adi_core.h>
#include <stdio.h>
#include <string.h>
#include "rpmsg_config.h"
#include <rpmsg_platform.h>
#include <rpmsg_lite.h>
#include <rpmsg_ns.h>
#include "adi_initialize.h"

#define ADI_RESOURCE_TABLE_INIT_MAGIC (0xADE0AD0E)
#define ADI_RESOURCE_TABLE_SHARC1_OFFSET (0x400) //1KiB
#define ADI_SHMEM_BUFFER_SIZE 0x00400

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
volatile struct adi_resource_table *adi_sharc0_resource_table;
volatile struct sharc_resource_table *sharc0_resource_table;
volatile struct adi_resource_table *adi_sharc1_resource_table;
volatile struct sharc_resource_table *sharc1_resource_table;

__attribute__ ((section (".l3_data_uncached")))
uint8_t arm_sharc0_buffer[ADI_SHMEM_BUFFER_SIZE];

__attribute__ ((section (".l3_data_uncached")))
uint8_t arm_sharc1_buffer[ADI_SHMEM_BUFFER_SIZE];

/* Static variables for rpmsg-lite */
struct rpmsg_lite_instance rpmsg_SHARC0_channel;
struct rpmsg_lite_ept_static_context sharc0_ctrl_endpoint_context;
struct rpmsg_lite_ept_static_context sharc0_tuning_endpoint_context;


struct rpmsg_lite_instance rpmsg_SHARC1_channel;

struct rpmsg_lite_ept_static_context sharc1_ctrl_endpoint1_context;
struct rpmsg_lite_ept_static_context sharc1_ctrl_endpoint2_context;
struct rpmsg_lite_ept_static_context sharc1_tuning_endpoint1_context;
struct rpmsg_lite_ept_static_context sharc1_tuning_endpoint2_context;

#define ARM_CTRL_EP1_ADDR 150
#define ARM_CTRL_EP2_ADDR 151
#define ARM_CTRL_EP3_ADDR 152

#define ARM_TUNING_EP1_ADDR 155
#define ARM_TUNING_EP2_ADDR 156
#define ARM_TUNING_EP3_ADDR 157

#define SHARC0_CTRL_EP_ADDR 160
#define SHARC0_TUNING_EP_ADDR 165

#define SHARC1_CTRL_EP1_ADDR 170
#define SHARC1_CTRL_EP2_ADDR 171
#define SHARC1_TUNING_EP1_ADDR 175
#define SHARC1_TUNING_EP2_ADDR 176

int32_t sharc0_to_arm_ctrl(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	return RL_RELEASE;
}
int32_t sharc0_to_arm_tuning(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	return RL_RELEASE;
}

int32_t sharc1_to_arm_ctrl1(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	return RL_RELEASE;
}

int32_t sharc1_to_arm_ctrl2(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	return RL_RELEASE;
}

int32_t sharc1_to_arm_tuning1(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	return RL_RELEASE;
}

int32_t sharc1_to_arm_tuning2(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
	return RL_RELEASE;
}

int sharc0_ready(void)
{
	return adi_sharc0_resource_table->initialized == ADI_RESOURCE_TABLE_INIT_MAGIC;
}

int sharc1_ready(void)
{
	return adi_sharc1_resource_table->initialized == ADI_RESOURCE_TABLE_INIT_MAGIC;
}


/*
 * Initialise rpmsg channel to SHARC core - master
 */
int rpmsg_init_channel_to_SHARCs(void)
{
	volatile struct rpmsg_lite_instance *rpmsg_instance;
	volatile struct rpmsg_lite_endpoint *rpmsg_ept;

	rpmsg_instance = rpmsg_lite_master_init(
			(void*)arm_sharc0_buffer,
			ADI_SHMEM_BUFFER_SIZE,
			RL_PLATFORM_ARM_SHARC0_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_SHARC0_channel);

	if (rpmsg_instance == RL_NULL)
		return -1;

	adi_sharc0_resource_table = (struct adi_resource_table *)
		((uint32_t)&__MCAPI_common_start);
	memcpy((void*)adi_sharc0_resource_table, (const void*)&rsc_tbl_local, sizeof(struct adi_resource_table));

	sharc0_resource_table = &adi_sharc0_resource_table->tbl;
	sharc0_resource_table->vring[0].da = (uint32_t) rpmsg_instance->rvq->vq_ring_mem;
	sharc0_resource_table->vring[1].da = (uint32_t) rpmsg_instance->tvq->vq_ring_mem;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC0_channel,
			ARM_CTRL_EP1_ADDR,
			&sharc0_to_arm_ctrl,
			NULL,
			&sharc0_ctrl_endpoint_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC0_channel,
			ARM_TUNING_EP1_ADDR,
			&sharc0_to_arm_tuning,
			NULL,
			&sharc0_tuning_endpoint_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	sharc0_resource_table->rpmsg_vdev.status = 7; //Signal other core that resource table has been initialised

	while (!sharc0_ready()) {
		/* Wait for sharc to complete init */
	}

	rpmsg_instance = rpmsg_lite_master_init(
			(void*)arm_sharc1_buffer,
			ADI_SHMEM_BUFFER_SIZE,
			RL_PLATFORM_ARM_SHARC1_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_SHARC1_channel);

	if (rpmsg_instance == RL_NULL)
		return -1;

	adi_sharc1_resource_table = (struct adi_resource_table *)
		((uint32_t)&__MCAPI_common_start + ADI_RESOURCE_TABLE_SHARC1_OFFSET);
	memcpy((void*)adi_sharc1_resource_table, (const void*)&rsc_tbl_local, sizeof(struct adi_resource_table));

	sharc1_resource_table = &adi_sharc1_resource_table->tbl;
	sharc1_resource_table->vring[0].da = (uint32_t) rpmsg_instance->rvq->vq_ring_mem;
	sharc1_resource_table->vring[1].da = (uint32_t) rpmsg_instance->tvq->vq_ring_mem;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC1_channel,
			ARM_CTRL_EP2_ADDR,
			&sharc1_to_arm_ctrl1,
			NULL,
			&sharc1_ctrl_endpoint1_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC0_channel,
			ARM_CTRL_EP3_ADDR,
			&sharc1_to_arm_ctrl2,
			NULL,
			&sharc1_ctrl_endpoint2_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC1_channel,
			ARM_TUNING_EP2_ADDR,
			&sharc1_to_arm_tuning1,
			NULL,
			&sharc1_tuning_endpoint1_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC1_channel,
			ARM_TUNING_EP3_ADDR,
			&sharc1_to_arm_tuning2,
			NULL,
			&sharc1_tuning_endpoint2_context);

	if (rpmsg_ept == RL_NULL)
		return -1;

	sharc1_resource_table->rpmsg_vdev.status = 7; //Signal other core that resource table has been initialised

	while (!sharc1_ready()) {
		/* Wait for sharc to complete init */
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int32_t loop=1;
	char msg[] = "hello";

	adi_initComponents();
	
	adi_core_enable(ADI_CORE_SHARC0);
	adi_core_enable(ADI_CORE_SHARC1);

	if (rpmsg_init_channel_to_SHARCs() == 0) {
		while(loop) {
			platform_time_delay(500);
			rpmsg_lite_send(
						&rpmsg_SHARC0_channel,
						&sharc0_ctrl_endpoint_context.ept,
						SHARC0_CTRL_EP_ADDR,
						msg, sizeof(msg), 0);
			platform_time_delay(100);
			rpmsg_lite_send(
						&rpmsg_SHARC1_channel,
						&sharc1_ctrl_endpoint1_context.ept,
						SHARC1_CTRL_EP1_ADDR,
						msg, sizeof(msg), 0);
			platform_time_delay(100);
			rpmsg_lite_send(
						&rpmsg_SHARC1_channel,
						&sharc1_ctrl_endpoint2_context.ept,
						SHARC1_CTRL_EP2_ADDR,
						msg, sizeof(msg), 0);

			// Send messages to tuning endpoints
			rpmsg_lite_send(
						&rpmsg_SHARC0_channel,
						&sharc0_tuning_endpoint_context.ept,
						SHARC0_TUNING_EP_ADDR,
						msg, sizeof(msg), 0);
			platform_time_delay(100);
			rpmsg_lite_send(
						&rpmsg_SHARC1_channel,
						&sharc1_tuning_endpoint1_context.ept,
						SHARC1_TUNING_EP1_ADDR,
						msg, sizeof(msg), 0);
			platform_time_delay(100);
			rpmsg_lite_send(
						&rpmsg_SHARC1_channel,
						&sharc1_tuning_endpoint2_context.ept,
						SHARC1_TUNING_EP2_ADDR,
						msg, sizeof(msg), 0);
		}
	}
	return 0;
}
