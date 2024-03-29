# Installation
Compiling applications for the node platform requires several components, their
installation is covered here.

# MCU SDK and commander tool
The SiLabs Gecko SDKs can be obtained by installing Simplicity Studio. Due to
SiLabs licensing terms, it is not possible to redistribute the files with the
project and it is necessary to download and install Simplicity Studio and the
Gecko SDK. This requires registration with SiLabs.

https://www.silabs.com/products/development-tools/software/simplicity-studio

Installing Simplicity Studio:
- Register with SiLabs and download `SimplicityStudio-5.tgz`
- Unpack Simplicity Studio\
  `tar xzvf SimplicityStudio-5.tgz -C ~`
- Run Simplicity Studio setup\
  `./setup.sh`
- Start Simplicity Studio\
  `./studio`
- Read and accept License agreements
- Log in with your SiLabs account
- Click `Help->Update Software`\
  (wait for update process to complete)
- Click `Install by Device`
- Choose the `SLWSTK6000B` Mighty Gecko Kit and click Next **twice**
- Make sure at least `Flex SDK 3.2.0.0` is selected and click Next\
  (It should be ok to use newer versions as well, though the SILABS_SDKDIR path may need adjustment)
- Accept licenses, click Finish, wait for completion and exit
- Add the path to commander to path - edit `~/.profile` and add:\
  `export PATH=$PATH:~/SimplicityStudio_v5/developer/adapter_packs/commander`

Makefiles require that the SILABS_SDKDIR variable point to the
SimplicityStudio_v5/developer/sdks/gecko_sdk_suite/v3.2 directory, so take
note of where SimplicityStudio_v5 ended up, if you followed the steps above,
then it should be stored at /home/user/SimplicityStudio_v5, which is the default
for SILABS_SDKDIR in the Makefiles.


# Compiler and toolchain

## Leveraging Simplicity Studio 5
At the time of writing, Simplicity Studio 5 also comes with the latest ARM toolchain provided by Arm Ltd (version 10-2020-q4-major).

If you followed the steps above and your Simplicity Studio installation ended up at `/home/user/SimplicityStudio_v5`, you can simply:

- edit `~/.profile` and add:\
  `export PATH=$PATH:~/SimplicityStudio_v5/developer/toolchains/gnu_arm/10.2_2020q4/bin`
- Refresh the environment and check version:\
  `source ~/.profile`\
  `arm-none-eabi-gcc -v`


## Directly form ARM Website
Alternatively, the toolchain can be obtained from
[ARM toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).

Version `8-2019-q3-update (Linux 64-bit)` is known to be a good working version, but these same steps apply if you prefer to use version `10-2020-q4-major`.

The ARM toolchain can be compiled from source, but installing the binary version
is easy and only requires that the *bin* directory of the toolchain be added
to the $PATH.
- Download `gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2`
- Unpack to `/opt`:\
  `sudo tar xjvf gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2 -C /opt/`
- Add to path - edit `~/.profile` and add:\
  `export PATH=$PATH:/opt/gcc-arm-none-eabi-8-2019-q3-update/bin`
- Refresh the environment and check version:\
  `source ~/.profile`\
  `arm-none-eabi-gcc -v`

It is also possible to use the compiler provided by your Linux distribution, but do note that the compiler in Ubuntu 18.04 failed to generate working code (a linker bug) and the one that came with Simplicity Studio 4 is relatively old and is not actively tested with the node platform.


# Headeredit
The headeredit tool is needed for embedding metadata into firmware images. The
tool can be obtained from [Headeredit](https://bitbucket.org/rebane/headeredit/src/master/).
The directory containing the compiled binary just needs to be in the $PATH or
the HEADEREDIT variable can be used to point to a custom location.

A debian package is also available from the Thinnect APT repository.
```
curl https://artifacts.thinnect.net/repository/certificates/pgp/APTPackager.pub.asc | sudo apt-key add --
echo "deb [arch=amd64,all] https://artifacts.thinnect.net/repository/apt-public/ stretch main" | sudo tee /etc/apt/sources.list.d/thinnect.public.list
sudo apt update
sudo apt install headeredit
```

# OpenOCD (optional)

Some boards(Thinnect tsb0 & tsb2) require OpenOCD for flashing.
OpenOCD is not needed for any of the official SiLabs development kits.

Currently the OpenOCD version distributed with Ubuntu is able to flash Series 1
chips, but not the User Signature page or Series 2 chips. A version capable of
flashing the User Signature page and Series 2 chips can be obtained from
[OpenOCD](https://bitbucket.org/thinnect/openocd/src/thinnect-patches).

A debian package is also available from the Thinnect APT repository.
```
curl https://artifacts.thinnect.net/repository/certificates/pgp/APTPackager.pub.asc | sudo apt-key add --
echo "deb [arch=amd64,all] https://artifacts.thinnect.net/repository/apt-public/ stretch main" | sudo tee /etc/apt/sources.list.d/thinnect.public.list
sudo apt update
sudo apt install openocd
```

To install support for the FTDI based programmer on the TSB board, also install `openocd-ftdi-thinnect`
```
sudo apt install openocd-ftdi-thinnect
```
