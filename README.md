# node-apps

Simple example applications for the Thinnect embedded node platform ecosystem.

The embedded node platform consists of a relatively generic embedded real-time
operating system, microcontroller/platform APIs and libraries and a simple
buildsystem. The platform is mostly focused for devices that use an 802.15.4
compatible wireless radio for communications. The specific 802.15.4 messaging
schema is based on and compatible with the [TinyOS](#tinyos) ActiveMessage principles.

# Platforms
Currently the examples only cover SiLabs Wireless Gecko based devices.
There are several SiLabs development boards that are supported, see
[dev-platforms](https://github.com/thinnect/dev-platforms/) or run `make` in one
of the example projects for a list of supported targets.

# Installation
Currently the build process supports only Linux systems, it has been tested
on Ubuntu 18.04, but should work on most Linux distributions, provided the
components installed from `*.deb` packages are handled manually or compiled from
source on non-deb distributions.

Installation is covered in detail in [INSTALL](INSTALL.md).

# APIs

## MistComm
Radio communications are performed throught the
[MistComm](https://github.com/thinnect/mist-comm/tree/proto) communications API.
The API is still a prototype, but is intended to support global addressing
without having to know the network hierarcy. It uses IEEE EUI-64 identifiers for
identifying endpoints (device identity, not just network address). The MistComm
layer is supposed to include an address translation layer to resolve the next
hop address for EUI-64 destinations, but this has not been finalized and for the
current examples, it only implements direct ActiveMessage addressing.
*The MistComm API will change*

## SPI/I2C
SPI/I2C interfaces are handled through RETARGET_Spi and RETARGET_I2C APIs, that
themselves are configured through retargetspiconfig.h and retargeti2cconfig.h.
This limits the application to a single instance of each bus, if needed,
additionalinstances need to be handled manually. RETARGET_Spi supports several
chip-select signals.

## Debug output
All the example applications and some libraries use the
[lll library](https://github.com/thinnect/lll) for logging to an UART. Each
platform has a designated Debug UART, which is configured in the platforms
`retargetserialconfig.h`. Loglevels can be tweaked by setting the BASE_LOG_LEVEL
and modifying the individual levels in each examples respective `loglevels.h` file.

# Embbedded operating system
The node platform is built around the ARM CMSIS RTOS abstaction layer and
therefore it would not be too complex to support different operating systems,
however currently only [FreeRTOS](#freertos) is used in practice and there may be some
direct hooks into the FreeRTOS kernel in the code.

# Application header
The compiled binary includes an information header that provides some details on
the binary and a checksum. Run the [headeredit](https://bitbucket.org/rebane/headeredit)
tool against the compiled binaries to see it.

________________________________________________________________________________

##### TinyOS
https://github.com/tinyos/tinyos-main
##### FreeRTOS
https://github.com/aws/amazon-freertos
