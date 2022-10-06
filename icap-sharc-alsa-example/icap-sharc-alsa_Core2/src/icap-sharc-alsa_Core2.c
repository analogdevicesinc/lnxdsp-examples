/*****************************************************************************
 * icap-sharc-alsa_Core2.c
 *****************************************************************************/

#include <sys/platform.h>
#include <sys/adi_core.h>
#include "adi_initialize.h"
#include "icap-sharc-alsa_Core2.h"
#include "icap_record_device.h"

#include "led_blink.h"


struct sharc_sync_msg dma_buffers;
volatile uint8_t dma_buf_initialized = 0;

/** 
 * If you want to use command program arguments, then place them in the following string. 
 */
char __argv_string[] = "";

int main(int argc, char *argv[])
{
	int32_t Result=0;
	int loop;
	/**
	 * Initialize managed drivers and/or services that have been added to 
	 * the project.
	 * @return zero on success 
	 */
	adi_initComponents();
	
	init_leds();
	test_leds();

	Result = rpmsg_init_channel_to_SHARC();
	if (Result) {
		turn_on_led(LED_ERROR);
	}

	init_rsc_tbl();
	while(!rsc_tbl_ready()){
		/* Wait for resource table to be initialized by ARM */
	}

	if (Result==0u)
	{
		Result = rpmsg_init_channel_to_ARM();
	}

	while(!rpmsg_lite_is_link_up(&rpmsg_ARM_channel)){
		/* Wait until ARM notifies the channel is up */
	}

	if (Result==0u)
	{
		Result = init_sharc_alsa_record();
	}

	while(1) {
		icap_loop(&icap_sharc_alsa);
	}
	return 0;
}

