# Installation
Compiling applications for the node platform requires several components, their
installation is covered here.

# Compiler and toolchain
Currently the platform uses the ARM toolchain provided by Arm Ltd, which can be
obtained from
[ARM toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).

"Version 8-2019-q3-update Linux 64-bit" is known to be a good working version.

The ARM toolchain can be compiled from source, but installing the binary version
is easy and only requires that the *bin* directory of the toolchain be added
to the $PATH.
	* Download `gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2`
	* Unpack to `/opt`:
		`sudo tar xjvf gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2 -C /opt/`
	* Add to path - edit `~/.profile` and add:
		`export PATH=$PATH:/opt/gcc-arm-none-eabi-8-2019-q3-update/bin`
	* Refresh the environment and check version:
		`source ~/.profile`
		`arm-none-eabi-gcc -v`

Alternatively it is possible to use the compiler from Simplicity Studio or
the one provided by you Linux distribution, but do note that the compiler in
Ubuntu 18.04 failed to generate working code (a linker bug) and the one in
Simplicity Studio is relatively old and is not actively tested with the node
platform.

# MCU SDK and commander tool
The SiLabs Gecko SDKs can be obtained by installing Simplicity Studio. Due to
SiLabs licensing terms, it is not possible to redistribute the files with the
project and it is necessary to download and install Simplicity Studio and the
Gecko SDK. This requires registration with SiLabs.

Essentially:
	* Register with SiLabs and download `SimplicityStudio-v4.tgz`
	* Unpack Simplicity Studio
		`tar xzvf SimplicityStudio-v4.tgz -C ~`
	* Run Simplicity Studio setup
		`./setup.sh`
	* Start Simplicity Studio
		`./run_studio.sh`
	* Read and accept License agreements
	* Log in with your SiLabs account
	* Click `Help->Update Software`
		(wait for update process to complete)
	* Click `Install by Device`
	* Choose the `SLWSTK6000B` Mighty Gecko Kit and click Next
	* Click Next
	* Make sure at least `Flex SDK 2.5.5` is selected and click Next
		(It should be ok to use newer versions as vell, though the SILABS_SDK
		path may need adjustment as well)
	* Accept licenses, click Finish, wait for completion and exit
	* Add the path to commander to path - edit `~/.profile` and add:
		`export PATH=$PATH:~/SimplicityStudio_v4/developer/adapter_packs/commander`

Makefiles require that the SILABS_SDK variable point to the
SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.5 directory, so take
note of where SimplicityStudio_v4 ended up, if you followed the steps above,
then it should be stored at /home/user/SimplicityStudio_v4, which is the default
for SILABS_SDK in the Makefiles.

# OpenOCD

Some boards require OpenOCD for flashing devices. Currently the version
distributed with Ubuntu is able to flash Series 1 chips, but not the User
Signature page or Series 2 chips. A version capable of flashing the User
Signature page and Series 2 chips can be obtained from
[OpenOCD](https://bitbucket.org/thinnect/openocd/src/thinnect-patches).

TODO: A debian package will be available from the Thinnect APT repository.

# ftdiutil
When using the ft2232d programmer, an additional utility is helpful for triggering
reset and managing the UARTs of the programmer. The tool can be obtained from
[ftdiutil}(https://bitbucket.org/rebane/ft2232d/src/master/software/ftdiutil/).
The directory containing the compiled binary just needs to be in the $PATH.

TODO: A debian package will be available from the Thinnect APT repository.

# Headeredit
The headeredit tool is needed for embedding metadata into firmware images. The
tool can be obtained from [Headeredit](https://bitbucket.org/rebane/headeredit/src/master/).
The directory containing the compiled binary just needs to be in the $PATH or
the HEADEREDIT variable can be used to point to a custom location.

TODO: A debian package will be available from the Thinnect APT repository.
