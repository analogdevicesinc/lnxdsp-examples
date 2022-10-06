#include "icap_record_device.h"

#include <sys/platform.h>
#include <sys/adi_core.h>
#include <drivers/sport/adi_sport.h>
#include <services/pdma/adi_pdma_2156x.h>

#include "icap-sharc-alsa_Core2.h"

#include "led_blink.h"

#define ICAP_SHARC_ALSA_EP_ADDRESS 20

struct icap_instance icap_sharc_alsa;

struct rpmsg_lite_ept_static_context icap_sharc_alsa_endpoint_context;

struct icap_device_buffer icap_sharc_alsa_src_buffer = {.buf_id = 1};

struct icap_subdevice_features sport_device_features = {
	.type = ICAP_DEV_RECORD,
	.src_buf_max = 0,
	.dst_buf_max = 1,
	.channels_min = CHANNELS,
	.channels_max = CHANNELS,
	.formats = ICAP_FMTBIT_S32_LE,
	.rates = ICAP_RATE_48000,
};


#define PLAYBACK_DMA 0
#define RECORD_DMA 1
struct adi_sport_info{
	uint32_t dir;
	uint32_t dma_interrupt;
	uint32_t dma_err_interrupt;
	uint32_t dma_status_interrupt;
	ADI_PDMA_REGS* pSportDMARegs;
};

static struct adi_sport_info sport_info[2]=
{
    {PLAYBACK_DMA,(uint32_t)INTR_SPORT4_A_DMA,(uint32_t)INTR_SPORT4_A_DMA_ERR,(uint32_t)INTR_SPORT4_A_STAT,(ADI_PDMA_REGS *)pREG_DMA10_DSCPTR_NXT},
	{RECORD_DMA,(uint32_t)INTR_SPORT4_B_DMA,(uint32_t)INTR_SPORT4_B_DMA_ERR,(uint32_t)INTR_SPORT4_B_STAT,(ADI_PDMA_REGS *)pREG_DMA11_DSCPTR_NXT},
};

void update_frags(struct icap_device_buffer *buf, uint32_t frags) {
	struct icap_buf_frags buf_frags;
	if (buf->descr.report_frags) {
		buf_frags.buf_id = buf->buf_id;
		buf_frags.frags = frags;
		icap_frag_ready(buf->icap, &buf_frags);
		buf->acks++;
	}
}

void inc_buf_pos (struct icap_device_buffer *buf, uint32_t size)
{
	int i;
	int8_t *addr;

	buf->offset += size;
	if (buf->offset >= buf->descr.buf_size) {
		buf->offset -= buf->descr.buf_size;
	}
	buf->frag_level += size;
	if (buf->frag_level >= buf->descr.frag_size) {
		buf->frag_level = 0;
		update_frags(buf, 1);
	}
}

int16_t get_s16 (struct icap_device_buffer *buf) {
	int16_t *addr;
	int16_t data;
	if (!buf->in_use){
		return 0;
	}
	addr = (int16_t *)((uint32_t)buf->descr.buf + buf->offset);
	data = *addr;
	inc_buf_pos(buf, sizeof(int16_t));
	return data;
}

void put_s16 (struct icap_device_buffer *buf, int16_t data) {
	int16_t *addr;
	if (!buf->in_use){
		return;
	}
	addr = (int16_t *)((uint32_t)buf->descr.buf + buf->offset);
	*addr = data;
	inc_buf_pos(buf, sizeof(int16_t));
}

int32_t get_s32 (struct icap_device_buffer *buf) {
	int32_t *addr;
	int32_t data;
	if (!buf->in_use){
		return 0;
	}
	// FIXME There is audio data miss alignment from unknown reason
	// Workaround: shift buffer addres by 1 `buf->offset + 1`
	addr = (int32_t *)((uint32_t)buf->descr.buf + buf->offset + 1);
	data = *addr;
	inc_buf_pos(buf, sizeof(int32_t));
	return data;
}

void put_s32 (struct icap_device_buffer *buf, int32_t data) {
	int32_t *addr;
	if (!buf->in_use){
		return;
	}
	// FIXME There is audio data miss alignment from unknown reason
	// Workaround: shift buffer addres by 1 `buf->offset + 1`
	addr = (int32_t *)((uint32_t)buf->descr.buf + buf->offset +1);
	*addr = data;
	inc_buf_pos(buf, sizeof(int32_t));
}

#define _MIN(a, b) ((a) < (b) ? (a) : (b))
#define _MAX(a, b) ((a) > (b) ? (a) : (b))

int32_t disable_cache(void) {
	uint32_t start, end;
	int32_t ret;
	adi_cache_set_disable_range(adi_cache_rr5);

	start = icap_sharc_alsa_src_buffer.descr.buf;
	end = start + icap_sharc_alsa_src_buffer.descr.buf_size;

	// Disable cache for the descriptors memory range
	ret = adi_cache_set_range ((void *)start,
						(void *)(end),
						adi_cache_rr5,
						adi_cache_noncacheable_range);
	// The delay is required after cache is disabled
	platform_time_delay(200);
	return ret;
}

int32_t icap_call_back(void *payload, uint32_t payload_len, uint32_t src, void *priv){
	struct icap_instance *icap = (struct icap_instance *)priv;
	union icap_remote_addr src_addr;
	src_addr.rpmsg_addr = src;
	return icap_put_msg(icap, &src_addr, payload, payload_len);
}

int create_icap_sharc_alsa_endpoint(void)
{
	struct rpmsg_lite_endpoint *rpmsg_ept;
	int ret;

	rpmsg_ept = rpmsg_lite_create_ept(
			&rpmsg_ARM_channel,
			ICAP_SHARC_ALSA_EP_ADDRESS + adi_core_id(),
			&icap_call_back,
			(void*)&icap_sharc_alsa,
			&icap_sharc_alsa_endpoint_context);
	if(rpmsg_ept == RL_NULL){
		return -1;
	}
	return 0;
}

int announce_icap_sharc_alsa_endpoint(void)
{
	struct rpmsg_lite_endpoint *rpmsg_ept = icap_sharc_alsa.transport.rpmsg_ept;
	int ret;

	ret = rpmsg_ns_announce(
			&rpmsg_ARM_channel,
			rpmsg_ept,
			"sharc-alsa",
			RL_NS_CREATE);
	if(ret != RL_SUCCESS){
		return -1;
	}
	return 0;
}

int remove_icap_sharc_alsa_endpoint(void)
{
	struct rpmsg_lite_endpoint *rpmsg_ept;
	int ret;

	ret = rpmsg_ns_announce(
			icap_sharc_alsa.transport.rpmsg_instance,
			icap_sharc_alsa.transport.rpmsg_ept,
			"sharc-alsa",
			RL_NS_DESTROY);
	if(ret) {
		return ret;
	}
	return rpmsg_lite_destroy_ept(
			icap_sharc_alsa.transport.rpmsg_instance,
			icap_sharc_alsa.transport.rpmsg_ept);
}


int32_t icap_sharc_alsa_get_subdevices(struct icap_instance *icap)
{
	/* We have only one record subdev */
	return 1;
}

/* Copy features from corresponding icap-linux-soport stream params*/
int32_t icap_sharc_alsa_get_subdevice_features(struct icap_instance *icap, uint32_t subdev_id, struct icap_subdevice_features *features)
{
	if (subdev_id >= 1){
		return -ICAP_ERROR_INVALID;
	}
	memcpy(features, &sport_device_features, sizeof(struct icap_subdevice_features));
	return 0;
}

int32_t icap_sharc_alsa_start(struct icap_instance *icap, uint32_t subdev_id)
{
	if (subdev_id >= 1){
		return -ICAP_ERROR_INVALID;
	}
	icap_sharc_alsa_src_buffer.acks = 0;
	icap_sharc_alsa_src_buffer.in_use = 1;
	return 0;
}

int32_t icap_sharc_alsa_stop(struct icap_instance *icap, uint32_t subdev_id)
{
	if (subdev_id >= 1){
		return -ICAP_ERROR_INVALID;
	}
	icap_sharc_alsa_src_buffer.in_use = 0;
	return 0;
}

int32_t icap_sharc_alsa_add_src(struct icap_instance *icap, struct icap_buf_descriptor *buf)
{
	return -ICAP_ERROR_NOT_SUP;
}

int32_t icap_sharc_alsa_add_dst(struct icap_instance *icap, struct icap_buf_descriptor *buf)
{
	memcpy(&icap_sharc_alsa_src_buffer.descr, buf, sizeof(struct icap_buf_descriptor));
	icap_sharc_alsa_src_buffer.offset = 0;
	icap_sharc_alsa_src_buffer.frag_level = 0;
	icap_sharc_alsa_src_buffer.icap = icap;
	icap_sharc_alsa_src_buffer.in_use = 0;
	disable_cache();
	return icap_sharc_alsa_src_buffer.buf_id;
}

int32_t icap_sharc_alsa_frag_ready_response(struct icap_instance *icap, int32_t buf_id)
{
	if(icap_sharc_alsa_src_buffer.buf_id == buf_id) {
		icap_sharc_alsa_src_buffer.acks--;
	}
	return 0;
}


struct icap_device_callbacks icap_sharc_alsa_callbacks = {
		.get_subdevices = icap_sharc_alsa_get_subdevices,
		.get_subdevice_features = icap_sharc_alsa_get_subdevice_features,
		.add_src = icap_sharc_alsa_add_src,
		.add_dst = icap_sharc_alsa_add_dst,
		.start = icap_sharc_alsa_start,
		.stop = icap_sharc_alsa_stop,
		.frag_ready_response = icap_sharc_alsa_frag_ready_response,
};

volatile uint8_t CallbackCount = 0;
extern struct sharc_sync_msg dma_buffers;
static void dmaInterruptHandler(uint32_t iid, void *handlerArg)
{
	struct adi_sport_info *info = (struct adi_sport_info*)handlerArg;
	void *dst, *src, *src2;
	uint32_t frames, channels;
	uint32_t i;
	int32_t *dma_buf;

	toggle_led(LED_HEARTBEAT);

	if(dma_buf_initialized){
		if(info->dir == RECORD_DMA){
			CallbackCount +=1;
			if(CallbackCount==1)
			{
				dma_buf = (int32_t *)dma_buffers.dma1_addr;
			   for(i=0;i<COUNT;i++)
			   {
				   put_s32(&icap_sharc_alsa_src_buffer, *dma_buf);
				   dma_buf++;

			   }
			}

			if(CallbackCount==2)
			{
				dma_buf = (int32_t *)dma_buffers.dma2_addr;
			   for(i=0;i<COUNT;i++)
			   {
				   put_s32(&icap_sharc_alsa_src_buffer, *dma_buf);
				   dma_buf++;
			   }
			   CallbackCount=0;
			}
		}else{
		}
	}

	info->pSportDMARegs->Status = ENUM_DMA_STAT_IRQDONE;
}

int init_sharc_alsa_record(void)
{
	int ret;

	// configure TX interrupts
	if (ADI_INT_SUCCESS != adi_int_InstallHandler(
			sport_info[RECORD_DMA].dma_interrupt,
			&dmaInterruptHandler,
			&sport_info[RECORD_DMA],
			false))
	{
		return -ICAP_ERROR_BUSY;
	}

	ret = create_icap_sharc_alsa_endpoint();
	if(ret) {
		return ret;
	}

	icap_sharc_alsa.transport.rpmsg_instance = &rpmsg_ARM_channel;
	icap_sharc_alsa.transport.rpmsg_ept = &icap_sharc_alsa_endpoint_context.ept;
	ret = icap_device_init(&icap_sharc_alsa, "record", &icap_sharc_alsa_callbacks, NULL);
	if(ret) {
		return ret;
	}

	ret = announce_icap_sharc_alsa_endpoint();
	if(ret) {
		return ret;
	}
	return 0;
}
