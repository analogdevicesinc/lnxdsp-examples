/* SPDX-License-Identifier: Apache-2.0 */
/*
  Example toggling LED upon receiving RPMsg-lite message
  Copyright (c) 2021-2022, Analog Devices, Inc.  All rights reserved.
*/

#include <sys/platform.h>
#include <sys/adi_core.h>
#include "adi_initialize.h"


int main()
{
	/**
	 * Initialize managed drivers and/or services that have been added to 
	 * the project.
	 * @return zero on success 
	 */
	adi_initComponents();
	
	/**
	 * The default startup code does not include any functionality to allow
	 * core 0 to enable core 1 and core 2. A convenient way to enable
	 * core 1 and core 2 is to use the adi_core_enable function.
	 */
	adi_core_enable(ADI_CORE_SHARC0);
	adi_core_enable(ADI_CORE_SHARC1);

	/* Begin adding your custom code here */

	return 0;
}


