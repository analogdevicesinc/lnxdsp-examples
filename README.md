# Yocto Linux for ADSP-SC5xx: Examples Repository

## Introduction
This repository contains a collection of examples related to the [Yocto Linux for ADSP-SC5xx processors](https://github.com/analogdevicesinc/lnxdsp-adi-meta/wiki).

Each example in this repository is self-contained.
Examples are provided in the following directory heirarchy **feature**/**example name**/**platform**. For example remoteproc/sharc-template-code/adsp-sc589-ezkit

All documentation related to each example is located in the **README.md** within the example's top-level folder.

## Branching Strategy / Where Are All The Examples?
This repository follows the same branching strategy as the other software repositories for the Yocto Linux for ADSP-SC5xx product. The **master** branch is not currently used. All examples for a given release are available in the branch for that release. For example the examples related to the 1.0.0 release of the Yocto Linux product are located in the **release/yocto-5.0.0** branch.

## List of examples

| Core 0 | Core 1 | Core 2 | Example name| Wiki/Status                                                                                                                                          |
| ------ | ------ | ------ | ----------- |------------------------------------------------------------------------------------------------------------------------------------------------------|
| N/A | Bare Metal | Bare Metal | icap | Part of ALSA-SHARC example https://github.com/analogdevicesinc/lnxdsp-adi-meta/wiki/SHARC%E2%80%90ALSA-Example                                       |
| N/A | Bare Metal | Bare Metal | icap-sharc-alsa-example | Part of ALSA-SHARC example https://github.com/analogdevicesinc/lnxdsp-adi-meta/wiki/SHARC%E2%80%90ALSA-Example                                       |
| N/A | Bare Metal | Bare Metal | mcapi/mcapi-message-example | MCAPI depricated in ADSP Linux >= 1.0.0, https://wiki.analog.com/resources/tools-software/linuxdsp/docs/linux-kernel-and-drivers/mcapi/mcapi_example |
| ARM/FreeRTOS | Bare Metal | Bare Metal | remoteproc/sharc-template-code |                                                                                                                                                      |
| N/A | Bare Metal | Bare Metal | rpmsg-lite  | rpmsg-lite port for SHARC processors. https://github.com/analogdevicesinc/lnxdsp-adi-meta/wiki/SHARC%E2%80%90ALSA-Example                            |
| ARM/FreeRTOS | Bare Metal | Bare Metal | rpmsg-lite-examples |

ARM/Linux examples located at [https://github.com/analogdevicesinc/rpmsg-examples](https://github.com/analogdevicesinc/rpmsg-examples)

## Licensing Information
Each example in this repository is self-contained and provides licensing information in a **LICENSE.md** file at the top level of the example.