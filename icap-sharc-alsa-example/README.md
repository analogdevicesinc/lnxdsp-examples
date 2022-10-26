## ICAP SHARC-ALSA example for Analog Devices Processors

This example implements two ICAP devices, one on SHARC0 and one on SHARC1, which communicate over rpmsg "sharc-alsa" endpoints. 
SHARC0 initializes and handles audio devices over i2c2 and i2s4, linux must have busses disabled.

### Supported Processor:
  * ADSP-SC598

### Tested with:
  * ADSP-SC598-SOM + CRR-EZKIT

### Build

  * After cloning the repo update the git submodules.
        git submodule init
        git submodule update
  * Open the project in CCES
  * Select Build project

### Usage

Example loop back thru sharc-alsa:<br>
hw:0,0 - playback device<br>
hw:1,0 - capture device

    arecord -c2 -fS32_LE -r48000 -D hw:1,0 | aplay -c2 -fS32_LE -r48000 -D hw:0,0

### Support

For support with issues related to this module, please post question in the [Analog Devices Linux for ADSP-SC5xx Processors Engineer Zone Forum](https://ez.analog.com/dsp/software-and-development-tools/linux-for-adsp-sc5xx-processors/f/q-a).

### Licensing

The source files included in this project are covered by the license described in the LICENSE file included in this module.  In addition to the source files included in this project the project also pulls in git submodules.  The licensing terms that apply to each submodule are stated in a separate LICENSE file found within that submodule.
