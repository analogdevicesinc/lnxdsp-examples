/*
 **
 ** sru_config.c source file generated on January 23, 2023 at 08:05:20.
 **
 ** Copyright (C) 2014-2023 Analog Devices Inc., All Rights Reserved.
 **
 ** This file is generated automatically based upon the options selected in
 ** the SRU Configuration editor. Changes to the SRU configuration should
 ** be made by changing the appropriate options rather than editing this file.
 **
 ** By default only the SRU registers with values different from their default
 ** reset value are initialized. Alternatively, macro SRU_FULL_INIT may be
 ** defined in the project settings to cause all SRU registers to be
 ** initialized.
 **
 */

#include <stdint.h>
#include <sys/platform.h>

int32_t adi_SRU_Init(void);

/*
 * Initialize the Signal Routing Unit
 */
int32_t adi_SRU_Init(void)
{
#if defined(SRU_FULL_INIT)
    /* Connections:
     *    DAI0_PB19_O - SPT2B_CLK_I
     *    DAI0_PB07_O - SPT0B_CLK_I
     *    DAI0_PB13_O - SPT1B_CLK_I
     *    DAI0_PB13_O - SPT1A_CLK_I
     *    DAI0_PB03_O - SPT0A_CLK_I
     *    DAI0_PB19_O - SPT2A_CLK_I
     */
    *pREG_DAI0_CLK0 = 0x252630c2uL;

    /* Connections:
     *    DAI0_LOW - SRC0_CLK_OP_I
     *    DAI0_LOW - SRC1_CLK_IP_I
     *    DAI0_LOW - SRC2_CLK_IP_I
     *    DAI0_LOW - SRC2_CLK_OP_I
     *    DAI0_LOW - SRC0_CLK_IP_I
     *    DAI0_LOW - SRC1_CLK_OP_I
     */
    *pREG_DAI0_CLK1 = 0x3def7bdeuL;

    /* Connections:
     *    DAI0_LOW - SPDIF0_TX_CLK_I
     *    DAI0_LOW - SRC3_CLK_IP_I
     *    DAI0_LOW - SRC3_CLK_OP_I
     */
    *pREG_DAI0_CLK2 = 0x3def7bdeuL;

    /* Connections:
     *    DAI0_LOW - SPDIF0_TX_HFCLK_I
     */
    *pREG_DAI0_CLK3 = 0x3def7bdeuL;

    /* Connections:
     *    DAI0_LOW - SPDIF0_TX_EXT_SYNC_I
     *    DAI0_LOW - PCG0_EXTCLKB_I
     *    DAI0_LOW - PCG0_SYNC_CLKA_I
     *    DAI0_LOW - PCG0_EXTCLKA_I
     *    DAI0_LOW - PCG0_SYNC_CLKB_I
     */
    *pREG_DAI0_CLK4 = 0x3def7bdeuL;

    /* Connections:
     *    DAI0_LOW - SPT3B_CLK_I
     *    DAI0_LOW - SPT3A_CLK_I
     */
    *pREG_DAI0_CLK5 = 0x3def7bdeuL;

    /* Connections:
     *    DAI0_PB09_O - SPT1A_D0_I
     *    DAI0_PB05_O - SPT0B_D0_I
     *    DAI0_PB06_O - SPT0B_D1_I
     *    DAI0_PB01_O - SPT0A_D0_I
     *    DAI0_PB02_O - SPT0A_D1_I
     */
    *pREG_DAI0_DAT0 = 0x08144040uL;

    /* Connections:
     *    DAI0_PB16_O - SPT2A_D1_I
     *    DAI0_PB10_O - SPT1A_D1_I
     *    DAI0_PB11_O - SPT1B_D0_I
     *    DAI0_PB15_O - SPT2A_D0_I
     *    DAI0_PB12_O - SPT1B_D1_I
     */
    *pREG_DAI0_DAT1 = 0x0f38b289uL;

    /* Connections:
     *    DAI0_PB01_O - SRC1_DAT_IP_I
     *    DAI0_PB17_O - SPT2B_D0_I
     *    DAI0_PB18_O - SPT2B_D1_I
     *    DAI0_PB01_O - SRC2_DAT_IP_I
     *    DAI0_PB01_O - SRC0_DAT_IP_I
     */
    *pREG_DAI0_DAT2 = 0x00000450uL;

    /* Connections:
     *    DAI0_PB01_O - SRC3_DAT_IP_I
     *    DAI0_PB01_O - SRC0_TDM_OP_I
     *    DAI0_PB01_O - SRC2_TDM_OP_I
     *    DAI0_PB01_O - SRC3_TDM_OP_I
     *    DAI0_PB01_O - SRC1_TDM_OP_I
     */
    *pREG_DAI0_DAT3 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB01_O - SPDIF0_TX_DAT_I
     */
    *pREG_DAI0_DAT4 = 0x00000000uL;

    /* Connections:
     *    DAI0_PB01_O - SPDIF0_RX_I
     */
    *pREG_DAI0_DAT5 = 0x00000000uL;

    /* Connections:
     *    DAI0_LOW - SPT3B_D0_I
     *    DAI0_LOW - SPT3A_D0_I
     *    DAI0_LOW - SPT3A_D1_I
     *    DAI0_LOW - SPT3B_D1_I
     */
    *pREG_DAI0_DAT6 = 0x00fbefbeuL;

    /* Connections:
     *    DAI0_PB14_O - SPT1A_FS_I
     *    DAI0_PB20_O - SPT2A_FS_I
     *    DAI0_PB08_O - SPT0B_FS_I
     *    DAI0_PB20_O - SPT2B_FS_I
     *    DAI0_PB04_O - SPT0A_FS_I
     *    DAI0_PB14_O - SPT1B_FS_I
     */
    *pREG_DAI0_FS0 = 0x2736b4e3uL;

    /* Connections:
     *    DAI0_LOW - SRC2_FS_IP_I
     *    DAI0_LOW - SRC0_FS_OP_I
     *    DAI0_LOW - SRC0_FS_IP_I
     *    DAI0_LOW - SRC1_FS_OP_I
     *    DAI0_LOW - SRC2_FS_OP_I
     *    DAI0_LOW - SRC1_FS_IP_I
     */
    *pREG_DAI0_FS1 = 0x3def7bdeuL;

    /* Connections:
     *    DAI0_LOW - SPDIF0_TX_FS_I
     *    DAI0_LOW - SRC3_FS_IP_I
     *    DAI0_LOW - SRC3_FS_OP_I
     */
    *pREG_DAI0_FS2 = 0x3def7bdeuL;

    /* Connections:
     *    DAI0_LOW - SPT3A_FS_I
     *    DAI0_LOW - SPT3B_FS_I
     */
    *pREG_DAI0_FS4 = 0x000003deuL;

    /* Connections:
     *    DAI0_LOW - DAI0_INT_8_I
     *    DAI0_LOW - DAI0_INT_7_I
     *    DAI0_LOW - DAI0_INT_6_I
     *    DAI0_LOW - DAI0_MISCA4_I
     *    DAI0_LOW - DAI0_MISCA5_I
     *    DAI0_LOW - DAI0_INV_MISCA4_I
     *    DAI0_LOW - DAI0_INT_9_I
     *    DAI0_LOW - DAI0_INV_MISCA5_I
     *    DAI0_LOW - DAI0_MISCA0_I
     *    DAI0_LOW - DAI0_MISCA1_I
     *    DAI0_LOW - DAI0_MISCA2_I
     *    DAI0_LOW - DAI0_MISCA3_I
     */
    *pREG_DAI0_MISC0 = 0x3def7bdeuL;

    /* Connections:
     *    DAI0_LOW - DAI0_INT_4_I
     *    DAI0_LOW - DAI0_INT_3_I
     *    DAI0_LOW - DAI0_INT_2_I
     *    DAI0_LOW - DAI0_INT_1_I
     *    DAI0_LOW - DAI0_INT_5_I
     *    DAI0_LOW - DAI0_INT_0_I
     */
    *pREG_DAI0_MISC1 = 0x3def7bdeuL;

    /* Connections:
     *    SPT0A_CLK_PBEN_O - DAI0_PBEN03_I
     *    SPT0A_D1_PBEN_O - DAI0_PBEN02_I
     *    SPT0A_D0_PBEN_O - DAI0_PBEN01_I
     *    SPT0B_D0_PBEN_O - DAI0_PBEN05_I
     *    SPT0A_FS_PBEN_O - DAI0_PBEN04_I
     */
    *pREG_DAI0_PBEN0 = 0x0e2482cauL;

    /* Connections:
     *    SPT0B_CLK_PBEN_O - DAI0_PBEN07_I
     *    SPT0B_D1_PBEN_O - DAI0_PBEN06_I
     *    SPT1A_D0_PBEN_O - DAI0_PBEN09_I
     *    SPT0B_FS_PBEN_O - DAI0_PBEN08_I
     *    SPT1A_D1_PBEN_O - DAI0_PBEN10_I
     */
    *pREG_DAI0_PBEN1 = 0x1348d30fuL;

    /* Connections:
     *    SPT2A_D0_PBEN_O - DAI0_PBEN15_I
     *    SPT1B_D1_PBEN_O - DAI0_PBEN12_I
     *    SPT1B_D0_PBEN_O - DAI0_PBEN11_I
     *    SPT1B_CLK_PBEN_O - DAI0_PBEN13_I
     *    SPT1B_FS_PBEN_O - DAI0_PBEN14_I
     */
    *pREG_DAI0_PBEN2 = 0x1a5545d6uL;

    /* Connections:
     *    SPT2A_D1_PBEN_O - DAI0_PBEN16_I
     *    SPT2B_D0_PBEN_O - DAI0_PBEN17_I
     *    SPT2B_D1_PBEN_O - DAI0_PBEN18_I
     *    SPT2B_FS_PBEN_O - DAI0_PBEN20_I
     *    SPT2B_CLK_PBEN_O - DAI0_PBEN19_I
     */
    *pREG_DAI0_PBEN3 = 0x1d71f79buL;

    /* Connections:
     *    SPT0A_D1_O - DAI0_PB02_I
     *    SPT0A_D0_O - DAI0_PB01_I
     *    SPT0A_FS_O - DAI0_PB04_I
     *    SPT0A_CLK_O - DAI0_PB03_I
     */
    *pREG_DAI0_PIN0 = 0x04c80a94uL;

    /* Connections:
     *    SPT0B_D1_O - DAI0_PB06_I
     *    SPT0B_D0_O - DAI0_PB05_I
     *    SPT0B_FS_O - DAI0_PB08_I
     *    SPT0B_CLK_O - DAI0_PB07_I
     */
    *pREG_DAI0_PIN1 = 0x04e84b96uL;

    /* Connections:
     *    SPT1B_D1_O - DAI0_PB12_I
     *    SPT1A_D1_O - DAI0_PB10_I
     *    SPT1B_D0_O - DAI0_PB11_I
     *    SPT1A_D0_O - DAI0_PB09_I
     */
    *pREG_DAI0_PIN2 = 0x03668c98uL;

    /* Connections:
     *    SPT2A_D1_O - DAI0_PB16_I
     *    SPT1B_FS_O - DAI0_PB14_I
     *    SPT2A_D0_O - DAI0_PB15_I
     *    SPT1B_CLK_O - DAI0_PB13_I
     */
    *pREG_DAI0_PIN3 = 0x03a714a3uL;

    /* Connections:
     *    SPT2B_D1_O - DAI0_PB18_I
     *    SPT2B_CLK_O - DAI0_PB19_I
     *    DAI0_PB01_O - INV_DAI0_PB19_I
     *    SPT2B_D0_O - DAI0_PB17_I
     *    DAI0_PB01_O - INV_DAI0_PB20_I
     *    SPT2B_FS_O - DAI0_PB20_I
     */
    *pREG_DAI0_PIN4 = 0x05694f9euL;
#endif /* defined(SRU_FULL_INIT) */

    return 0;
}

