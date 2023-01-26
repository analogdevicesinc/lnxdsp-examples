/*****************************************************************************
 * sc584-freertos-led-toggle_Core0.c
 *****************************************************************************/

/* Standard includes. */
#include <stdlib.h>
#include <sys/platform.h>
#include <sys/adi_core.h>
#include "adi_initialize.h"

/* Kernel includes. */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/* Application includes. */
#include "rpmsg-led-toggle-task.h"

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

	/* When using FreeRTOS calls to SSL/DD cannot be made until the scheduler is active.
	To ensure that devices are initialized before tasks are started place your code
	in vApplicationDaemonTaskStartupHook() located in FreeRTOSHooks.c */

	/* Create a FreeRTOS task to run when the scheduler starts */
	BaseType_t xReturned;
	TaskHandle_t startupTaskHandle;
	xReturned = xTaskCreate( rpmsgLedToggleTask,
				"Startup Task",
				configMINIMAL_STACK_SIZE,
				NULL,
				tskIDLE_PRIORITY + 2,
				&startupTaskHandle );

	if( xReturned != pdPASS )
	{
		abort();
	}

	/* Start the scheduler. */
	vTaskStartScheduler();

	return 0;
}

