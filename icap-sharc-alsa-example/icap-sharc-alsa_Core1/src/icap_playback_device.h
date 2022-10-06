#ifndef ICAP_PLAYBACK_DEVICE_H_
#define ICAP_PLAYBACK_DEVICE_H_

#include <rpmsg_platform.h>
#include <rpmsg_lite.h>
#include <rpmsg_ns.h>
#include <icap_device.h>

#define COUNT 50
#define CHANNELS 2

struct icap_device_buffer {
	struct icap_buf_descriptor descr;
	uint32_t offset;
	uint32_t frag_level;
	uint32_t buf_id;
	struct icap_instance *icap;
	uint32_t in_use;
	int32_t acks;
};

extern struct rpmsg_lite_instance rpmsg_ARM_channel;
extern struct icap_instance icap_sharc_alsa;
extern struct icap_device_buffer icap_sharc_alsa_src_buffer;

int init_sharc_alsa_playback(void);

int16_t get_s16 (struct icap_device_buffer *buf);
void put_s16 (struct icap_device_buffer *buf, int16_t data);
int32_t get_s32 (struct icap_device_buffer *buf);
void put_s32 (struct icap_device_buffer *buf, int32_t data);

#endif /* ICAP_PLAYBACK_DEVICE_H_ */
