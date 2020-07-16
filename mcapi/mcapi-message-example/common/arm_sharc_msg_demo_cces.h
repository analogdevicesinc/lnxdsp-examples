/*****************************************************************************
 * @file  arm_sharc_msg_demo_cces.h
 *
 * Copyright (c) 2019, Analog Devices, Inc.  All rights reserved.
 *
 *****************************************************************************/

#ifndef ARM_SHARC_MSG_DEMO_CCES_H
#define ARM_SHARC_MSG_DEMO_CCES_H

#include <services/mcapi/mcapi.h>

/* This macro indicates whether error checking should be done for all the
 * MCAPI API calls and the message size
 */
#define API_RETURN_VALUE_CHECK 	1

#if ( API_RETURN_VALUE_CHECK )
#define DEBUG_BUF_SIZE			128u
#define MCAPI_ERROR_CHECK(mcapi_status, psContext, coreID)\
do {\
	char errorStringBuff[DEBUG_BUF_SIZE];\
	if ( (MCAPI_SUCCESS != (mcapi_status) ) && ( MCAPI_PENDING != (mcapi_status) ) )\
	{\
		mcapi_display_status( (mcapi_status), errorStringBuff, sizeof(errorStringBuff) );\
		printf( "[CORE%d]: MCAPI Error %s, status = %u [%s]\n",\
				(coreID),\
				(psContext),\
				(mcapi_status),\
				errorStringBuff );\
		exit( coreID );\
	}\
} while (0)
#define MSG_SIZE_CHECK( check_size, target_size, coreID ) \
do {\
	if ( (check_size) != (target_size) )\
	{\
		printf( "[CORE%d]: Msg recv size 0x%x is out of target size 0x%x\n",\
			    (coreID),\
			    (check_size),\
			    (target_size));\
		exit( coreID );\
	}\
}while (0)
#else
#define MCAPI_ERROR_CHECK( mcapi_status, psContext, coreID )
#define MSG_SIZE_CHECK( check_size, target_size, coreID )
#endif


/* MCAPI Domain number. Zero is the only supported domain at present. */
#define DOMAIN 					0u

/**
 * MCAPI Node numbers for the processor cores.
 */
#define NODE_CORE_0 			0u
#define NODE_CORE_1 			1u
#define NODE_CORE_2 			2u

/**
 * MCAPI Port numbers used by the application.
 * Numbering should start at MCAPI_NUM_RESERVED_PORTS, which is an
 * implementation-defined constant and may be >= zero.
 */
#define CORE0_PORT_NUM_1		101u
#define CORE0_PORT_NUM_2		200u
#define CORE1_PORT_NUM			5u
#define CORE2_PORT_NUM			6u

/**
 * Maximum size of the buffer array within the message struct
 */
#define BUF_SIZE				64u

/**
 * Commands for execution, Common between ARM and SHARC Cores
 */
enum DSP_COMMAND {
  DSP_CMD_TERMINATE = 1,
  DSP_CMD_RESPONSE,
  DSP_CMD_EXECUTE
};

struct DSP_MSG {
	enum DSP_COMMAND cmd;
	uint32_t buffSize;
	char buffer[BUF_SIZE];
};

#endif /* ARM_SHARC_MSG_DEMO_CCES_H*/

