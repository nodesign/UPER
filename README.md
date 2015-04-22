# UPER firmware
UPER, originally writted by [8devices](https://github.com/8devices/UPER), is the firmware currently used for [WeIO](https://github.com/nodesign/weio).

The original version uses LPCXpresso, with some specific libraries, such as RedLib (a CodeRed proprietary implementation of the Newlib). It does mean that LPCXpresso is required to compile the firmware.

This version uses newlib, and can be compiled in a standard way using *make*.

# How to build the firmware
In order to build the firmware, two dependancies must be included : [SFP](https://github.com/giedriusm/SFP) and [MemoryManager](https://github.com/8devices/MemoryManager)

````
git clone https://github.com/ks156/UPER
cd UPER
git checkout newlib
git clone https://github.com/8devices/MemoryManager
git clone https://github.com/giedriusm/SFP
````

A GCC for ARM toolchain is also required. It can be downloaded and installed from here : [GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded/+download). 
In addition, *python* must also be installed in the system. It is required to compute the checksum of the vector section. Without a correct checksum, the bootloader will not recognize the firmware as valid. 

Once the system is ready, simply type *make* from the terminal. This will compile the firmware, and output a binary file *UPER-Release.bin*
