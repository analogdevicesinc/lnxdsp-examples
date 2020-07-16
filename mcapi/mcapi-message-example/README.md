## mcapi message example for Analog Devices Processors

This example is a mcapi message example running on SHARC Cores to communicate with
the ARM Core which is running the Analog Devices Yocto Linux for ADSP-SC5xx product. 

### Supported Processor:
  * ADSP-SC573
  * ADSP-SC584
  * ADSP-SC589

### Tested with:
  * ADSP-SC573 EZ-KIT BOARD
  * ADSP-SC584 EZ-KIT BOARD
  * ADSP-SC589 EZ-KIT BOARD
  * ADSP-SC589 MINI BOARD

### User Configuration Macros:
    API_RETURN_VALUE_CHECK which is defined in arm_sharc_msg_demo_cces.h
    determines whether the return value from the MCAPI APIs should be checked.
    It is recommended that the error values are always checked in the debug
    builds and at least once in the release ones.

### Hardware Setup:

	Use default software switch settings.

### Usage

This project should be checked out and built automatically by Yocto.
Please refer to the documentation at [Yocto Linux on wiki.analog.com](https://wiki.analog.com/resources/tools-software/linuxdsp) for more details.

### Support

For support with issues related to this module, please post question in the [Analog Devices Linux for ADSP-SC5xx Processors Engineer Zone Forum](https://ez.analog.com/dsp/software-and-development-tools/linux-for-adsp-sc5xx-processors/f/q-a).

### Licensing

For details of licensing related to this project please refer to the LICENSE.md file included in this module.
