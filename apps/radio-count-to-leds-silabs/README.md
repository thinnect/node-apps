# RadioCountToLeds

RadioCountToLeds is a simple radio broadcast demo application. It runs a 4 Hz
counter and broadcasts the value every time it is updated. When the application
hears a packet from another node, it displays the value from the packet using
its LEDs. The number of displayed bits depends on the number of LEDs on a
particular platform (max 8). The application is intended to test and demonstrate
how the system is set up, how threads, timers and the radio work.

The application is compatible with the TinyOS RadioCountToLeds application
and protocol, it has been briefly tested against an IRIS node.

https://github.com/tinyos/tinyos-main/tree/master/apps/RadioCountToLeds

# Work in progress
The application mainly demonstrates radio functionality on top of the MistComm
API, but the MistComm API still has some issues and therefore the way it is used
will change before the 1.0 release. Keep checking this exaple app to keep up
with changes and improvements.

# Platforms
The application has been tested and should work with the following platforms:
 * Thinnect TestSystemBoard0 (tsb0)
 * Thinnect TestSystemBoard2 (tsb2)
 * SiLabs Thunderboard Sense (thunderboard)
 * SiLabs Thunderboard Sense 2 (thunderboard2)
 * SiLabs BRD4001A + BRD4161A (brd4161a-mb)
 * SiLabs BRD4001A + BRD4162A (brd4162a-mb)
 * SiLabs BRD4001A + BRD4304A (brd4304a-mb)

TODO Series2 boards:
 * SiLabs BRD4001A + BRD4180A (brd4180a-mb)

# Build
Standard build options apply, check the main [README](../../README.md).
Additionally the device address can be set at compile time, see
[the next chapter](#device_address_/_signature) for details.

# Device address / signature

Devices are expected to carry a device signature using the format
specified in https://github.com/thinnect/euisiggen and
https://github.com/thinnect/device-signature. The device then derives a short
network address using the lowest 2 bytes of the EUI64 contained in the
signature.

If the device does not have a signature, then the application will
initialize the radio with the value defined with DEFAULT_AM_ADDR. For example
to set the address to 0xABCD in the firmware for a Thunderboard Sense 2, make
can be called `make thunderboard2 DEFAULT_AM_ADDR=0xABCD`. It is necessary to
call `make clean` manually when changing the `DEFAULT_AM_ADDR` value as the
buildsystem is unable to recognize changes of environment variables.
