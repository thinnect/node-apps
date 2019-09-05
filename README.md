# node-apps

Simple example applications for the Thinnect embedded node platform ecosystem.

The embedded node platform consists of a relatively generic embedded real-time
operating system, microcontroller/platform APIs and libraries and a simple
buildsystem. The platform is mostly focused for devices that use an 802.15.4
compatible wireless radio for communications. The specific 802.15.4 messaging
schema is based on and compatible with the
[TinyOS](https://github.com/tinyos/tinyos-main) ActiveMessage principles.

# Applications

## Blink
Blink is a simple application that blinks LEDs at different frequencies. It
demonstrates OS setup and using threads, mutexes and timers.

See [Blink readme](apps/blink-silabs/README.md) for details.

## RadioCountToLeds
RadioCountToLeds is a simple application that broadcasts a counter value to the
network and displays received counter values on LEDs. It demonstrates OS setup,
using threads, mutexes, timers and the MistComm Radio API.

See [RadioCountToLeds readme](apps/radio-count-to-leds-silabs/README.md) for details.

# Platforms
Currently the examples only cover some SiLabs Wireless Gecko based devices.
There are several SiLabs development boards that are supported, see
[dev-platforms](https://github.com/thinnect/dev-platforms/) or run `make` in one
of the example projects for a list of supported targets.

# Installation
Currently the build process supports only Linux systems, it has been tested
on Ubuntu 18.04, but should work on most Linux distributions, provided the
components installed from `*.deb` packages are handled manually or compiled from
source on non-deb distributions.

Installation is covered in detail in [INSTALL](INSTALL.md).

# Setup
This repository relies on several dependencies that are all publically available
on [GitHub](https://github.com) and they have been linked to the repository as
submodules. After cloning the repository, the submodules need to be initialized
and updated.
```
git clone git@github.com:thinnect/node-apps.git
cd node-apps
git submodule init
git submodule update
```
Initially the submodules will be pointing to the correct commits/states of their
respective repositories, but in a detached-head state. The actual branches they
should be checked out to are listed in the [gitmodules](.gitmodules) file.
Install [git-submodule-gizmos](https://github.com/raidoz/git-submodule-gizmos)
to get a tool for easier management of the branches.

# APIs

## MistComm
Radio communications are performed throught the
[MistComm](https://github.com/thinnect/mist-comm/tree/proto) communications API.
The API is still a prototype, but is intended to support global addressing
without having to know the network hierarcy. It uses IEEE EUI-64 identifiers for
identifying endpoints (device identity, not just network address). The MistComm
layer is supposed to include an address translation layer to resolve the
next-hop address for EUI-64 destinations, but this has not been finalized and
for the current examples, it only implements direct ActiveMessage addressing.
*The MistComm API will change*

## SPI/I2C
SPI/I2C interfaces are handled through RETARGET_Spi and RETARGET_I2C APIs, that
themselves are configured through retargetspiconfig.h and retargeti2cconfig.h.
This limits the application to a single instance of each bus, if needed,
additional instances need to be handled manually. RETARGET_Spi supports several
chip-select signals.

## Debug output
All the example applications and some libraries use the
[lll library](https://github.com/thinnect/lll) for logging to an UART. Each
platform has a designated Debug UART, which is configured in the platforms
`retargetserialconfig.h`. Loglevels can be tweaked by setting the BASE_LOG_LEVEL
and modifying the individual levels in each examples respective `loglevels.h` file.

A good tool for receiving the output from a serial port is
[serial-logger](https://github.com/thinnect/serial-logger).

# Embbedded operating system
The node platform is built around the ARM CMSIS RTOS abstaction layer and
therefore it would not be too complex to support different operating systems,
however currently only [FreeRTOS](https://github.com/aws/amazon-freertos) is
used in practice and there may be some direct hooks into the FreeRTOS kernel in
the code.

# Application header
The compiled binary includes an information header that provides some details on
the binary and a checksum. Run the [headeredit](https://bitbucket.org/rebane/headeredit)
tool against the compiled binaries to see it.

# Build and flash

To build an example application, enter the applications directory and build
it for one of the supported platforms with `make PLATFORM_NAME` and flash to the
device with `make PLATFORM_NAME install`.

For example to install on a Thunderboard Sense, run:
```
make thunderboard
make thunderboard install
```

The thunderboard target and other SiLabs development targets use the built-in
J-Link programmer by default and only a single board can be connected to the PC
at a time.

The Thinnect TestSystemBoard has a built-in ft2232d based programmer and uses
OpenOCD and ftdiutil (see [INSTALL](INSTALL.md) for details). A specific device
can be selected by specifying the FTDI_ID variable:
```
make tsb install FTDI_ID=3
```

# Coding rules

Code in this repository must conform to the
[BARR-C:2018](https://barrgroup.com/Embedded-Systems/Books/Embedded-C-Coding-Standard)
coding rules with the following permitted exceptions:
- Lines are allowed to be up to 120 characters long, keeping them shorter is
  however recommended.
- static module-level variables must be prefixed with m_, global ones with g_.
