/*
 * Copyright (c) 2020, Analog Devices, Inc.  All rights reserved.
 *
 *   Functions for remoteproc feature
 */

#include <stdio.h>
#include <services/int/adi_int.h>
#include "adsp-sc5xx-remoteproc.h"

static void ExecuteIdleLoop()
{
	do {
		asm("idle;");
	} while (1);
}

void SecHandler( uint32_t iid, void *handlerArg )
{
	uint32_t SID;
	/* sec CSID register */
	volatile uint32_t *pCSID, *pEND;

	SID = *pREG_SEC0_CSID1;
	/* Write the read value back to the SEC_CSID[n] register
	 * to send the acknowledge signal to the SEC that the core
	 * has accepted and begun processing for the interrupt request */
	*(uint32_t *)0x3108945C = SID;

	/*
	 * Check the SEC request is the core request for SHARC core1
	 */
	if (*pREG_RCU0_MSG & BITM_RCU_MSG_C1CRR)
	{
		/* clear the CRR bit */
		*pREG_RCU0_MSG_CLR |= BITM_RCU_MSG_C1CRR;
	} else if (*pREG_RCU0_MSG & BITM_RCU_MSG_C2CRR)
	{
		/* clear the CRR bit */
		*pREG_RCU0_MSG_CLR |= BITM_RCU_MSG_C2CRR;
	}

	SEC_RESET = true;

	/* END the SEC */
	*pREG_SEC0_END = SID;

}

void prvRemoteProcProbe( ADI_CORE_ID __core_id )
{
	int result = 0;
	SEC_RESET = false;
	int softirq;

	if (__core_id == ADI_CORE_SHARC0)
		softirq = INTR_SOFT0;
	else if (__core_id == ADI_CORE_SHARC1)
		softirq = INTR_SOFT1;

	if ( 0 != adi_int_InstallHandler(
			softirq,                 /* IID for SEC interrupt */
			&SecHandler, 				/* our SEC interrupt handler */
			NULL,     					/* pass the connection as its argument */
			true ) )                	/* enable the interrupt immediately */
	{
		result = -1;
	}
	else
	{
		result = 0;
	}

}

void prvRemoteProcRemove( ADI_CORE_ID __core_id )
{
	int result = 0;
	int softirq;
	unsigned long rcu_msg_idle;

	if (__core_id == ADI_CORE_SHARC0)
	{
		softirq = INTR_SOFT0;
		rcu_msg_idle = BITM_RCU_MSG_C1IDLE;
	} else if (__core_id == ADI_CORE_SHARC1)
	{
		softirq = INTR_SOFT1;
		rcu_msg_idle = BITM_RCU_MSG_C2IDLE;
	}

	if ( 0 != adi_int_UninstallHandler( softirq) )
	{
		result = -1;
	}
	else
	{
		result = 0;
	}

	*pREG_RCU0_MSG_SET |= rcu_msg_idle;
	/* Execute idle */
	ExecuteIdleLoop();
}