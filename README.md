# Yocto Linux for ADSP-SC5xx: Examples Repository

## Introduction
This repository contains a collection of examples related to the [Yocto Linux for ADSP-SC5xx processors](https://wiki.analog.com/resources/tools-software/linuxdsp).
More information related to this repository can be found on the [Examples Documentation page](https://wiki.analog.com/resources/tools-software/linuxdsp/docs/examples).

Each example in this repository is self-contained.
Examples are provided in the following directory heirarchy **feature**/**example name**/**platform**. For example remoteproc/sharc-template-code/adsp-sc589-ezkit

All documentation related to each example is located in the **README.md** within the example's top-level folder.

## Branching Strategy / Where Are All The Examples?
This repository follows the same branching strategy as the other software repositories for the Yocto Linux for ADSP-SC5xx product. The **master** branch is not currently used. All examples for a given release are available in the branch for that release. For example the examples related to the 1.0.0 release of the Yocto Linux product are located in the **release/yocto-1.0.0** branch.

## Licensing Information
Each example in this repository is self-contained and provides licensing information in a **LICENSE.md** file at the top level of the example.