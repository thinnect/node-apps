# node-apps

Simple example applications for the Thinnect embedded node platform ecosystem.

The embedded node platform consists of a relatively generic embedded real-time
operating system, microcontroller/platform APIs and libraries and a simple
buildsystem. The platform is mostly focused for devices that use an 802.15.4
compatible wireless radio for communications. The specific 802.15.4 messaging
schema is based on and compatible with the TinyOS[^1] ActiveMessage principles.

Currently the examples only cover SiLabs Wireless Gecko based devices.

# Installation
Currently the build process supports only Linux systems, it has been tested
on Ubuntu 18.04, but should reasonably work on most Linux distributions,
provided the components installed from `*.deb` packages are handled manually
or compiled from source on non-deb distributions.

Installation is covered in detail in [INSTALL.md]

# Embbedded operating system
The node platform is built around the ARM CMSIS RTOS abstaction layer and
therefore it would not be too complex to support different operating systems,
however currently only FreeRTOS is used in practice and there may be some
direct hooks into the FreeRTOS kernel in the code. This example project in its
current state relies exlusively on FreeRTOS.

# Debug output
All the example applications use the [lll library](https://github.com/thinnect/lll)
for logging to an UART. Loglevels can be tweaked by setting the BASE_LOG_LEVEL
and modifying the individual levels in each examples respective `loglevels.h` file.

# Application header
The compiled binary includes an information header that provides some details on
the binary and a checksum. Run the `headeredit` tool against the compiled binaries
to see it.

________________________________________________________________________________

[^1]: [TinyOS](https://github.com/tinyos/tinyos-main)
