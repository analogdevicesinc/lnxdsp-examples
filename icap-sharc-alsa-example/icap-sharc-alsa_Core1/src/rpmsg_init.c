#include <sys/platform.h>
#include <sys/adi_core.h>

#include <rpmsg_platform.h>
#include <rpmsg_lite.h>
#include <rpmsg_ns.h>
#include <icap_device.h>

#include "icap-sharc-alsa_Core1.h"

#define ADI_RESOURCE_TABLE_INIT_MAGIC (0xADE0AD0E)
#define ADI_RESOURCE_TABLE_SHARC1_OFFSET (0x400) //1KiB
/*
 * Expected resource table layout in the shared memory.
 * Initialized by ARM.
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
			1, 								 /* version */
			1, /* number of table entries */
			{0, 0,},					 /* reserved fields */
		},
		.tbl.offset = {offsetof(struct sharc_resource_table, rpmsg_vdev),
		},
		.tbl.rpmsg_vdev = {RSC_VDEV, /* virtio dev type */
			7, /* it's rpmsg virtio */
			1, /* kick sharc0 */
			/* 1<<0 is VIRTIO_RPMSG_F_NS bit defined in virtio_rpmsg_bus.c */
			1<<0, 0, 0, 0, /* dfeatures, gfeatures, config len, status */
			2, /* num_of_vrings */
			{0, 0,}, /* reserved */
		},
		.tbl.vring = {
			{(uint32_t)-1, VRING_ALIGN, 512, 1, 0}, /* da allocated by remoteproc driver */
			{(uint32_t)-1, VRING_ALIGN, 512, 1, 0}, /* da allocated by remoteproc driver */
		},
};

/*
 * Two resource tables, one for each core.
 * The ___MCAPI_common_start address is defined in app.ldf
 */
extern "asm" struct adi_resource_table ___MCAPI_common_start;
static struct adi_resource_table *adi_resource_table;
static struct sharc_resource_table *resource_table;

/* Static variables for rpmsg-lite */
struct rpmsg_lite_instance rpmsg_ARM_channel;

/*
 * The ___MCAPI_sharc1_start address is defined in app.ldf
 */
extern "asm" uint8_t ___MCAPI_sharc1_start;
uint8_t *rpmsg_lite_shared_memory = &___MCAPI_sharc1_start;

/* Static variables for rpmsg-lite */
struct rpmsg_lite_instance rpmsg_SHARC_channel;
struct rpmsg_lite_ept_static_context sharc_sync_endpoint_context;
#define SHARC_SYNC_EP_ADDR_CORE0 155
#define SHARC_SYNC_EP_ADDR_CORE1 156

int32_t sharc_sync_call_back(void *payload, uint32_t payload_len, uint32_t src, void *priv){
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
void vring_get_descriptor_range(struct fw_rsc_vdev_vring *vring, struct _mem_range *range){
	struct vring_desc *desc = (struct vring_desc *)vring->da;
	range->start = (uint32_t)desc;
	range->end = (uint32_t)desc + vring_size(vring->num, vring->align);
}
void vring_get_buffer_range(struct fw_rsc_vdev_vring *vring, struct _mem_range *range){
	struct vring_desc *desc = (struct vring_desc *)vring->da;
	uint32_t num = 2 * vring->num; // vring0 descriptor has pointer to buffers for both vrings
	range->start = (uint32_t)desc->addr;
	range->end = (uint32_t)desc->addr + num * (RL_BUFFER_PAYLOAD_SIZE +16);
}

void init_rsc_tbl(void) {
	// The delay is required after cache is disabled
	platform_time_delay(200);

	switch(adi_core_id()){
	case ADI_CORE_ARM:
		return;
	case ADI_CORE_SHARC0:
		adi_resource_table = &___MCAPI_common_start;
		resource_table = &___MCAPI_common_start.tbl;
		break;
	case ADI_CORE_SHARC1:
		adi_resource_table = (struct adi_resource_table *)
			((uint32_t)&___MCAPI_common_start + ADI_RESOURCE_TABLE_SHARC1_OFFSET);
		resource_table = &adi_resource_table->tbl;
		break;
	default:
		// should never happen
		break;
	}

	/* Don't initialize if remoteproc driver has already */
	if(strcmp((const char *)adi_resource_table->tag, (const char *)rsc_tbl_local.tag)){
		*adi_resource_table = rsc_tbl_local;

		switch(adi_core_id()){
		case ADI_CORE_ARM:
			return;
		case ADI_CORE_SHARC0:
			adi_resource_table->tbl.rpmsg_vdev.notifyid = 1;
			adi_resource_table->tbl.vring[0].notifyid = 1;
			adi_resource_table->tbl.vring[1].notifyid = 1;
			break;
		case ADI_CORE_SHARC1:
			adi_resource_table->tbl.rpmsg_vdev.notifyid = 2;
			adi_resource_table->tbl.vring[0].notifyid = 2;
			adi_resource_table->tbl.vring[1].notifyid = 2;
			break;
		default:
			// should never happen
			break;
		}

	}
}

int rsc_tbl_ready(void) {
	/* 0x1 acknowledge, 0x2 driver found, 0x4 driver ready*/
	return resource_table->rpmsg_vdev.status == 7;
}

/*
 * Initialize rpmsg channel to ARM core
 */
int rpmsg_init_channel_to_ARM(void){
	struct rpmsg_lite_instance *rpmsg_instance;
	adiCacheStatus status;
	struct _mem_range range0;
	struct _mem_range range1;

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
	// The delay is required after cache is disabled
	platform_time_delay(200);

	// Read vring buffer memory range
	// vring1 has its own descriptors but share buffers with vring0
	vring_get_buffer_range(&resource_table->vring[0], &range1);
	// Disable cache for the vring buffer range
	status = adi_cache_set_range ((void *)range1.start,
						(void *)(range1.end),
						adi_cache_rr7,
						adi_cache_noncacheable_range);
	// The delay is required after cache is disabled
	platform_time_delay(200);

	rpmsg_instance = rpmsg_lite_remote_init(
			&resource_table->rpmsg_vdev,
			RL_PLATFORM_SHARC_ARM_LINK_ID,
			RL_SHM_VDEV,
			&rpmsg_ARM_channel);
	if(rpmsg_instance == RL_NULL){
		return -1;
	}

	adi_resource_table->initialized = ADI_RESOURCE_TABLE_INIT_MAGIC;
	return 0;
}

/*
 * Initialize rpmsg channel to SHARC core - master
 */
int rpmsg_init_channel_to_SHARC(void){
	struct rpmsg_lite_instance *rpmsg_instance;
	struct rpmsg_lite_endpoint *rpmsg_ept;
	adiCacheStatus status;
	uint32_t addr;

	rpmsg_instance = rpmsg_lite_master_init(
			rpmsg_lite_shared_memory,
			128*1024,
			RL_PLATFORM_SHARC_SHARC_LINK_ID,
			0,
			&rpmsg_SHARC_channel);
	if(rpmsg_instance == RL_NULL){
		return -1;
	}

	if (adi_core_id() == ADI_CORE_SHARC0){
		addr = SHARC_SYNC_EP_ADDR_CORE0;
	} else if (adi_core_id() == ADI_CORE_SHARC1){
		addr = SHARC_SYNC_EP_ADDR_CORE1;
	} else {
		return -1;
	}

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_SHARC_channel,
			addr,
			&sharc_sync_call_back,
			NULL,
			&sharc_sync_endpoint_context);
	if(rpmsg_ept == RL_NULL){
		return -1;
	}

	return 0;
}

int rpmsg_SHARC_sync(struct sharc_sync_msg *msg){
	uint32_t remote_addr;
	if (adi_core_id() == ADI_CORE_SHARC0){
		remote_addr = SHARC_SYNC_EP_ADDR_CORE1;
	} else {
		remote_addr = SHARC_SYNC_EP_ADDR_CORE0;
	}

	return rpmsg_lite_send(
			&rpmsg_SHARC_channel,
			&sharc_sync_endpoint_context.ept,
			remote_addr,
			(char*)msg, sizeof(struct sharc_sync_msg), 0);
}
