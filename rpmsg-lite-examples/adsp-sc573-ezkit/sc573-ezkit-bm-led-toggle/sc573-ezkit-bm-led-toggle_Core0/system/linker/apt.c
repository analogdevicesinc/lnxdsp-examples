/* Copyright (c) 2015-2021, Analog Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*!
* @brief     Abstract page table for ADSP-SC57x parts.
*
* @details
*            This is the table which defines the MMU attributes of memory.
*
*            The default distribution of L2 and L3 memory between the ARM and
*            the SHARC cores is chosen to give similar amounts of memory to each
*            core. However, the memory requirements in your application for the
*            ARM and SHARC cores may not be equal. In addition, the L3 memory
*            specified is that which is available on the ADSP-SC573 EZ-KIT.
*
*            If you have a different amount of L3 memory, or change the
*            distribution of L2 or L3 memory between the cores, please note
*            that these changes must also be reflected in the LDF files for
*            your SHARC projects, and in the LD file used for your ARM project.
*/

#include <runtime/mmu/adi_mmu.h>

/* See adi_mmu.h for definitions of the ADI_MMU_* memory class macros. */

#ifndef ADI_UNCACHED_SHARC_MEMORY
#define SHARC_L2 ADI_MMU_RO_CACHED
#define SHARC_L3 ADI_MMU_RO_CACHED
#else
#define SHARC_L2 ADI_MMU_RO_UNCACHED
#define SHARC_L3 ADI_MMU_RO_UNCACHED
#endif

/* ADI_CHANGES is defined for the default page table objects to ensure the
 * default symbols are weak.  Users should not define this macro when building
 * their application-specific page table.
 */
#ifdef ADI_CHANGES
const adi_mmu_AbstractPageEntry _adi_mmu_absPageTable[] __attribute__ ((weak)) =
#else
const adi_mmu_AbstractPageEntry _adi_mmu_absPageTable[] =
#endif
{
    /* THE ADDRESS RANGES IN THIS TABLE MUST BE IN ASCENDING ORDER. */

    /* L2 ROM */
    { 0x00000000u, 0x00007FFFu, ADI_MMU_RO_CACHED           }, /* 32KB L2 ROM */

    /* L2 Cache Controller MMR space */
    { 0x10000000u, 0x10000FFFu, ADI_MMU_RW_DEVICE           }, /* 4KB L2CC MMRs */

    /* L2 SRAM */
    { 0x20000000u, 0x20003FFFu, ADI_MMU_RW_STRONGLY_ORDERED }, /* 16KB ICC */
    { 0x20004000u, 0x20007FFFu, ADI_MMU_RW_UNCACHED         }, /* 16KB MCAPI ARM */
    { 0x20008000u, 0x2000FFFFu, ADI_MMU_RO_UNCACHED         }, /* 32KB MCAPI SHARC */
    { 0x20010000u, 0x2001FFFFu, ADI_MMU_RW_UNCACHED         }, /* 64KB ARM uncached L2 */
#if defined(__ADSPSC570__) || defined(__ADSPSC572__)
    { 0x20020000u, 0x2008FFFFu, ADI_MMU_WB_CACHED           }, /* 448KB ARM cached L2 */
    { 0x20090000u, 0x200FFFFFu, SHARC_L2                    }, /* 448KB SHARC0 & 8KB boot code working area */
#else
    { 0x20020000u, 0x2007FFFFu, ADI_MMU_WB_CACHED           }, /* 384KB ARM cached L2 */
    { 0x20080000u, 0x200FFFFFu, SHARC_L2                    }, /* 512KB SHARC1, SHARC0 & 8KB boot code working area */
#endif /* __ADSPSC570__ || __ADSPSC572__ */

    { 0x24000000u, 0x24001FFFu, ADI_MMU_RW_DEVICE           }, /* 8KB OTP space */

    /* L1 memory of SHARC0 in MP space */
    { 0x28240000u, 0x2825FFFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB SHARC0 L1B0 */
    { 0x282C0000u, 0x282DFFFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB SHARC0 L1B1 */
    { 0x28300000u, 0x2830FFFFu, ADI_MMU_RO_UNCACHED         }, /* 64KB SHARC0 L1B2 */
    { 0x28380000u, 0x2838FFFFu, ADI_MMU_RO_UNCACHED         }, /* 64KB SHARC0 L1B3 */

#if __NUM_SHARC_CORES__ > 1
    /* L1 memory of SHARC1 in MP space */
    { 0x28A40000u, 0x28A5FFFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB SHARC1 L1B0 */
    { 0x28AC0000u, 0x28ADFFFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB SHARC1 L1B1 */
    { 0x28B00000u, 0x28B0FFFFu, ADI_MMU_RO_UNCACHED         }, /* 64KB SHARC1 L1B2 */
    { 0x28B80000u, 0x28B8FFFFu, ADI_MMU_RO_UNCACHED         }, /* 64KB SHARC1 L1B3 */
#endif /* __NUM_SHARC_CORES__ */

    /* System MMR space */
    { 0x30000000u, 0x3FFFFFFFu, ADI_MMU_RW_DEVICE           }, /* 256MB System MMRs */

    /* Asynchronous memory */
    { 0x40000000u, 0x4FFFFFFFu, ADI_MMU_RW_UNCACHED         }, /* 4*64MB Async memory banks0-3 */

    /* SPI Flash memory-mapped address space */
    { 0x60000000u, 0x60FFFFFFu, ADI_MMU_RO_DEVICE           }, /* 16MB SPI flash */

    /* Dynamic Memory Controller 0 (DMC0) 256MB SDRAM */
#if defined(__ADSPSC572__)
    { 0x80000000u, 0x87FFFFFFu, SHARC_L3                    }, /* 128MB DDR-A */
    { 0x88000000u, 0x8FFFFFFFu, ADI_MMU_WB_CACHED           }, /* 128MB DDR-A */
#elif defined(__ADSPSC573__)
    { 0x80000000u, 0x88FFFFFFu, SHARC_L3                    }, /* 144MB DDR-A */
    { 0x89000000u, 0x8FFFFFFFu, ADI_MMU_WB_CACHED           }, /* 112MB DDR-A */
#endif /* __ADSPSC572__ || __ADSPSC573__ */

};

/* ADI_CHANGES is defined for the default page table objects to ensure the
 * default symbols are weak.  Users should not define this macro when building
 * their application-specific page table.
 */
#ifdef ADI_CHANGES
const uint32_t _adi_mmu_absPageTableSize __attribute__ ((weak)) = sizeof(_adi_mmu_absPageTable) / sizeof(_adi_mmu_absPageTable[0]);
#else
const uint32_t _adi_mmu_absPageTableSize = sizeof(_adi_mmu_absPageTable) / sizeof(_adi_mmu_absPageTable[0]);
#endif
