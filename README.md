# UPER firmware
UPER, originally written by [8devices](https://github.com/8devices/UPER) is the firmware currently used for [WeIO](https://github.com/nodesign/weio).

The original version uses LPCXpresso, with specific libraries, such as RedLib (a CodeRed proprietary implementation of Newlib). It does mean that LPCXpresso is required to compile the firmware.

This version uses newlib, and can be compiled in a standard way using *make*.

In addition, it has been customized for WeIO.

# Directory structure
In this version, the directory structure differs from the [original UPER version](https://github.com/8devices/UPER). The structure is the following :

````
|- UPER
|  |-- UPER : Core files
|  |   \-- src : main, USB descriptor, startup and system timer
|  |   |   \-- CDC : USB CDC driver 
|  |   |   |-- Driver : MCU initialization
|  |   |   |-- Modules : Peripheral and specific devices
|  |   |   |-- System : Cortex M0 system files
|  |   |-- inc : header files
|  |-- UserFunctions : C files for users defined functions
|  |-- MemoryManager : Dynamic Memory managing for embedded systems
|  |-- SFP : Simple Function Protocol
|  |-- drivers : Windows USB CDC drivers
|  |-- utils : Utilities
           
````

The directory *UserFunctions* allows users to create and link a custom function easily. Read *README.md* file in the *UserFunctions* directory for more informations.

# How to build the firmware
In order to build the firmware, two dependencies must be included : [SFP](https://github.com/giedriusm/SFP) and [MemoryManager](https://github.com/8devices/MemoryManager)

````
git clone https://github.com/ks156/UPER
cd UPER
git checkout newlib
git submodule init
git submodule update
````

A GCC for ARM toolchain is also required. It can be downloaded and installed from here : [GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded/+download). 
In addition, *python* must also be installed in the system. It is required to compute the checksum of the vector section. Without a correct checksum, the bootloader will not recognize the firmware as valid. 

Once the system is ready, simply type *make* from the terminal. This will compile the firmware, and output a binary file *UPER-Release.bin*

