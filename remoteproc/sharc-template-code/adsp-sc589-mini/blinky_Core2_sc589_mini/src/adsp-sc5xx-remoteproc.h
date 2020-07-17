/*
 * Copyright (c) 2020, Analog Devices, Inc.  All rights reserved.
 *
 * adsp-sc5xx-remoteproc.h
 */

#ifndef ADSP_SC5XX_REMOTEPROC_H_
#define ADSP_SC5XX_REMOTEPROC_H_

#define BITP_RCU_MSG_C2CRR                   14            /*  Core 2 Reset Request Flag */
#define BITP_RCU_MSG_C1CRR                   13            /*  Core 1 Reset Request Flag */
#define BITP_RCU_MSG_C0CRR                   12            /*  Core 0 Reset Request Flag */
#define BITM_RCU_MSG_C2CRR                  (_ADI_MSK_3(0x00004000,0x00004000UL, uint32_t  ))    /*  Core 2 Reset Requested */
#define BITM_RCU_MSG_C1CRR                  (_ADI_MSK_3(0x00002000,0x00002000UL, uint32_t  ))    /*  Core 1 Reset Requested */
#define BITM_RCU_MSG_C0CRR                  (_ADI_MSK_3(0x00001000,0x00001000UL, uint32_t  ))    /*  Core 0 Reset Requested */

void prvRemoteProcProbe( ADI_CORE_ID __core_id );
void prvRemoteProcRemove( ADI_CORE_ID __core_id );
bool SEC_RESET;

#endif /* ADSP_SC5XX_REMOTEPROC_H_ */
