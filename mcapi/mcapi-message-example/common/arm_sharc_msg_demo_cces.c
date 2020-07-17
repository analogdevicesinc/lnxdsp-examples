/*****************************************************************************
 * @file arm_sharc_msg_demo_cces.c
 *
 * Copyright (c) 2019, Analog Devices, Inc.  All rights reserved.
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>

#include <sys/platform.h>
#include <sys/adi_core.h>
#include <mcapi.h>

#include "adi_initialize.h"
#include "arm_sharc_msg_demo_cces.h"

#define REMOTE_NODE				NODE_CORE_0
#if defined( CORE1 )
#define LOCAL_PORT				CORE1_PORT_NUM
#define REMOTE_PORT				CORE0_PORT_NUM_1
#elif defined( CORE2 )
#define LOCAL_PORT				CORE2_PORT_NUM
#define REMOTE_PORT				CORE0_PORT_NUM_2
#else
#error "Unknown processor family as an invalid Core ID"
#endif

int main(void)
{
	mcapi_status_t mcapi_status;
	mcapi_endpoint_t local_ep;
	mcapi_endpoint_t remote_ep;

	struct DSP_MSG xrMsg;
	struct DSP_MSG xsMsg;
	size_t msg_size = 0;

	int coreID = adi_core_id();
	int srcID, dstID;
	unsigned int round;

	/* Initialize managed drivers and/or services. */
	if ( 0 != adi_initComponents() )
	{
		printf( "[CORE%d]: Error initializing components\r\n", coreID );
	}

	/* Create the local endpoints. */
	local_ep = mcapi_endpoint_create( LOCAL_PORT, &mcapi_status );
	MCAPI_ERROR_CHECK( mcapi_status, "create endpoint", coreID );

	/* Get the remote endpoints. */
	remote_ep = mcapi_endpoint_get( DOMAIN,
									REMOTE_NODE,
									REMOTE_PORT,
									MCAPI_TIMEOUT_INFINITE,
									&mcapi_status);
	MCAPI_ERROR_CHECK( mcapi_status, "get endpoint", coreID );

	printf( "[CORE%d]: localEp = %u remoetEp = %u \n", coreID, local_ep, remote_ep );

	/* while to receive, process and send back the message date. */
	do {
		mcapi_msg_recv( local_ep,
						&xrMsg,
						sizeof(struct DSP_MSG),
						&msg_size,
						&mcapi_status );
		MCAPI_ERROR_CHECK( mcapi_status, "msg_recv", coreID );

		if ( MCAPI_SUCCESS == mcapi_status ) {
			MSG_SIZE_CHECK( msg_size, sizeof(struct DSP_MSG), coreID );

			/* Process the received message and fill in the to-be-sent message. */
			xrMsg.buffer[xrMsg.buffSize] = '\0';
			sscanf( xrMsg.buffer, "%*s%*s%d%*s%*s%*s%d%*s%d", &srcID, &dstID, &round);

			/* Counting number ++ and fill it into the to-be-sent message. */
			snprintf( xsMsg.buffer,
					  BUF_SIZE,
					  "hello core %d message from core %d - %u",
					  dstID,
					  srcID,
					  (++round) );
			xsMsg.buffSize = strlen( xsMsg.buffer );
			xsMsg.cmd = DSP_CMD_EXECUTE;
			msg_size = sizeof(struct DSP_MSG);

			/* Send a message back. */
			mcapi_msg_send( local_ep,
							remote_ep,
							&xsMsg,
							msg_size,
							0,
							&mcapi_status);
			MCAPI_ERROR_CHECK( mcapi_status, "msg_send",  coreID );
		}
	} while ( xrMsg.cmd != DSP_CMD_TERMINATE  );

	/* Recycle the recourses and Finalize the MCAPI. */
	printf("[CORE%d] Test %u rounds Passed\r\n", coreID, round);
	mcapi_endpoint_delete( local_ep, &mcapi_status );
	MCAPI_ERROR_CHECK( mcapi_status, "del_endpoint", coreID );
	mcapi_finalize( &mcapi_status );
	MCAPI_ERROR_CHECK( mcapi_status, "finalize", coreID );

	return 0;
}
