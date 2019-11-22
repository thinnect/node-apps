# TempSender

TempSender is a simple radio broadcast demo application that sends a fake
temperature reading in the Mist Resource data format.

The data is sent without encryption, so make sure to allow insecure
communications in the gateway configuration.

# Work in progress
The application mainly demonstrates radio functionality on top of the MistComm
API, but the MistComm API still has some issues and therefore the way it is used
will change before the 1.0 release. Keep checking this exaple app to keep up
with changes and improvements.

# Platforms
The application has been tested and should work with the following platforms:
 * Thinnect TestSystemBoard (tsb)
 * SiLabs BRD4001A + BRD4304A (brd4304a-mb)

# Build
Standard build options apply, check the main [README](../../README.md).
Additionally the device address can be set at compile time, see
[the next chapter](#device_address_/_signature) for details.

# Device address / signature

Devices are expected to carry a device signature using the format specified in
https://github.com/thinnect/euisiggen
and
https://github.com/thinnect/device-signature
.
The device then derives a short network address using the lowest 2 bytes of the
EUI64 contained in the signature.

If the device does not have a signature, then the application will
initialize the radio with the value defined with DEFAULT_AM_ADDR. For example
to set the address to 0xABCD in the firmware for a TestSystemBoard, make
can be called `make tsb DEFAULT_AM_ADDR=0xABCD`. It is necessary to
call `make clean` manually when changing the `DEFAULT_AM_ADDR` value as the
buildsystem is unable to recognize changes of environment variables.
