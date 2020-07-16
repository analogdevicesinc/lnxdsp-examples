# Overview
This example is a simple LED blinking example that uses the Remoteproc framework on ADSP-SC5XX platform.<br>
With the configurations below, users can generate one SHARC ldr file which is loaded by the ADI Remoteproc to start and stop SHARC cores.<br>

# Hardware Requirements
## Supported platforms 
* ADSP-SC573 EZ-KIT BOARD
* ADSP-SC584 EZ-KIT BOARD
* ADSP-SC589 EZ-KIT BOARD
* ADSP-SC589 MINI BOARD

## Hardware Setup
No hardware configuration required.<br>

# CCES Configuration
## Configure to generate ldr file
Set below configurations in Project->Properties window:<br>
* C/C++ Build -> Settings -> Build Artifact -> Artifact Type: `Loader File`
* C/C++ Build -> Settings -> Tools Settings -> CrossCore SHARC Loader -> General -> Boot mode: `SPI master`
* C/C++ Build -> Settings -> Tools Settings -> CrossCore SHARC Loader -> General -> Boot format: `Binary`
* C/C++ Build -> Settings -> Tools Settings -> CrossCore SHARC Loader -> General -> Boot code: `1`
* C/C++ Build -> Settings -> Tools Settings -> CrossCore SHARC Loader -> Additional Options: `-MaxBlockSize 2048`<br>

Remember to set the "Executable Files" to point to your dxe files in core1 or core2 box.<br>
* C/C++ Build -> Settings -> Tools Settings -> CrossCore SHARC Loader -> Executable Files

# SHARC Remoteproc Framework Introduction
Two APIs are implemented for users to call for:<br>
* void prvRemoteProcProbe( ADI_CORE_ID __core_id );<br>
* void prvRemoteProcRemove( ADI_CORE_ID __core_id );<br>

One bool parameter is defined to indicate the ARM core's message through SEC interface. Currently the ARM core will raise a core reset interrupt through SEC.<br>
* bool SEC_RESET;

## How to use Remoteproc Framework in Your Own SHARC Project:
1. Call the "prvRemoteProcProbe(coreid);" at the begin of your main function. In this probe function, SEC_RESET will be set to false and one specific interrupt (INTR_SOFT0 for SHARC Core0, INTR_SOFT1 is for SHARC Core1) will be registered to handle the reset request from ARM core. It is not recommended to do any modification in this function.<br>
2. Create a while loop or a task to check the "bool SEC_RESET" value. Once this bool value is set to true, call the prvRemoteProcRemove() to uninstall the interrupt handler. SEC_RESET will set to true once the SEC_HANDLR is called, which means the SHARC core reset request is triggered by ARM core, and the program should release all related resources and execute the Idle loop.<br>
### One simple example code:
```
/* main function of the example */
int main(int argc, char *argv[])
{
	....
	/* Remoteproc framework initialization */
	prvRemoteProcProbe(CoreID);
	/* Create the Remoteproc state check task */
	createTask(checkTask);
	....
}
/* Body of the Remoteproc state check task */
void checkTask()
{
	while(1)
	{
		if (SEC_RESET) {
				/* call Remoteproc Remove to free all requested resources on SHARC core and execute the IDLE loop */
				prvRemoteProcRemove(nCoreID);
		} else {
			....
		}
	}
}
```