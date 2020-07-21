/*
 * Copyright (c) 2020, Analog Devices, Inc.  All rights reserved.
 *
 * LED blinking example
 */

#include <sys/platform.h>
#include <sys/adi_core.h>
#include "adi_initialize.h"
#include "adsp-sc5xx-remoteproc.h"

#define TIME_DELAY 0x5FFFFF

#ifdef __ADSPSC589__
#if 0
/* ADSP-SC589 EZKIT board
 * pins 15:13, port E
 */
#define DIRSET 0xe000
#define DATTGL 0xe000
#else
/* SHARC Audio board
 * pins 3:1, port D
 */
#define DIRSET 0x000e
#define DATTGL 0x000e
#endif
#elif defined(__ADSPSC584__)
/* pins 3:1, port E */
#define DIRSET 0x000e
#define DATTGL 0x000e
#elif defined(__ADSPSC573__)
/* pins 15,13,1, port E */
#define DIRSET 0xa002
#define DATTGL 0xa002
#endif

uint32_t var[2];

int main(int argc, char *argv[])
{
	int i,k=0;
	ADI_CORE_ID nCoreID = adi_core_id();

	/* Remoteproc Initialize */
	prvRemoteProcProbe(nCoreID);

    *pREG_PORTD_FER      = 0;   // GPIO mode
    *pREG_PORTD_DIR_SET  = DIRSET;

    while(1)
	{
    	if (SEC_RESET) {
    			*pREG_PORTD_DATA_CLR = DIRSET;
    	    	/* Remoteproc Remove */
    	    	prvRemoteProcRemove(nCoreID);
    	} else {
    		*pREG_PORTD_DATA_TGL  = DATTGL;
    		for(i=0; i<TIME_DELAY; i++)
    			k++;
    		var[0] = 0xdead;
    		var[1] = 0xbeef;
    	}
	}

	return 0;
}
