# This is the main makefile for RadioCountToLeds firmware

# _______________________ User overridable configuration _______________________

PROJECT_NAME            ?= radiocounttoleds

VERSION_MAJOR           ?= 1
VERSION_MINOR           ?= 0
VERSION_PATCH           ?= 0
VERSION_DEVEL           ?= "-dev"

RADIO_CHANNEL           ?= 13

# Set device address at compile time for cases where a signature is not present
DEFAULT_AM_ADDR         ?= 1
# Set destination address at compile time
DESTINATION_GATEWAY_ADDRESS ?= 0xFFFF

# No bootloader, app starts at 0
APP_START               = 0

# Common build options - some of these should be moved to targets/boards
CFLAGS                  += -Os -g -Wall -std=c99
CFLAGS                  += -specs=nano.specs -specs=nosys.specs
CFLAGS                  += -ffunction-sections -fdata-sections -ffreestanding -fsingle-precision-constant -Wstrict-aliasing=0
CFLAGS                  += -D__START=main -D__STARTUP_CLEAR_BSS
CFLAGS                  += -DVTOR_START_LOCATION=$(APP_START)
LDFLAGS                 += -nostartfiles -Wl,--gc-sections -Wl,--relax -Wl,-Map=$(@:.elf=.map),--cref -Wl,--wrap=atexit
LDLIBS                  += -lgcc
INCLUDES                += -Xassembler

# If set, disables asserts and debugging, enables optimization
RELEASE_BUILD           ?= 0
# Set the lll verbosity base level
CFLAGS                  += -DBASE_LOG_LEVEL=0xFFFF

# Enable debug messages
VERBOSE                 ?= 0
# Disable info messages
#SILENT                  ?= 1

# This project contains several Makefiles that reference the project root
ROOT_DIR                ?= $(abspath ../..)
ZOO                     ?= $(ROOT_DIR)/zoo
# Destination for build results
BUILD_BASE_DIR          ?= build
# Mark the default target
DEFAULT_BUILD_TARGET    ?= $(PROJECT_NAME)

# Configure how image is programmed to target device
PROGRAM_IMAGE           ?= $(BUILD_DIR)/$(PROJECT_NAME).bin
PROGRAM_DEST_ADDR       ?= $(APP_START)

# Silabs SDK location and version, due to licensing terms, the SDK is not
# distributed with this project and must be installed with Simplicity Studio.
# The variable needs to point at the subdirectory with the version number, set
# it in Makefile.private or through the environment.
SILABS_SDKDIR           ?= $(HOME)/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.5

# Pull in the developer's private configuration overrides and settings
-include Makefile.private

# _______________________ Non-overridable configuration _______________________

BUILD_DIR                = $(BUILD_BASE_DIR)/$(BUILD_TARGET)
BUILDSYSTEM_DIR         := $(ZOO)/thinnect.node-buildsystem/make
PLATFORMS_DIRS          := $(ZOO)/thinnect.node-buildsystem/make $(ZOO)/thinnect.dev-platforms/make
PHONY_GOALS             := all clean
TARGETLESS_GOALS        += clean
UUID_APPLICATION        := d709e1c5-496a-4d31-8957-f389d7fdbb71

VERSION_BIN             := $(shell printf "%02X" $(VERSION_MAJOR))$(shell printf "%02X" $(VERSION_MINOR))$(shell printf "%02X" $(VERSION_PATCH))
VERSION_STR             := "$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)"$(VERSION_DEVEL)
BUILD_TIMESTAMP         := $(shell date '+%s')

FREERTOS_DIR ?= $(ZOO)/FreeRTOS-Kernel
FREERTOS_INC = -I$(FREERTOS_DIR)/include \
               -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3 \
               -I$(ZOO)/thinnect.cmsis-freertos/silabs
FREERTOS_SRC = $(wildcard $(FREERTOS_DIR)/*.c) \
               $(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c \
               $(FREERTOS_DIR)/portable/MemMang/heap_5.c \
               $(ZOO)/thinnect.cmsis-freertos/silabs/cmsis_os2.c \
               $(ZOO)/thinnect.cmsis-freertos/silabs/freertos_evr.c \
               $(ZOO)/thinnect.cmsis-freertos/silabs/FreeRTOS_Tick_Config.c
CMSIS_CONFIG_DIR ?= $(ZOO)/thinnect.cmsis-freertos/silabs/config

RAIL_INC = -I$(SILABS_SDKDIR)/platform/radio/rail_lib/hal \
           -I$(SILABS_SDKDIR)/platform/radio/rail_lib/common \
           -I$(SILABS_SDKDIR)/platform/radio/rail_lib/protocol/ieee802154 \
           -I$(SILABS_SDKDIR)/platform/radio/rail_lib/plugin/pa-conversions \

RAIL_SRC =  $(SILABS_SDKDIR)/platform/radio/rail_lib/hal/hal_common.c

# MoteXML components
MOTEXML_DIR ?= $(ZOO)/prolab.motexml
DTTYPES_DIR ?= $(ZOO)/prolab.dt-types
MOTEXML_INC = -I$(MOTEXML_DIR)/lib -I$(DTTYPES_DIR)
MOTEXML_SRC = $(MOTEXML_DIR)/lib/MLE.c $(MOTEXML_DIR)/lib/MLD.c $(MOTEXML_DIR)/lib/MLI.c
CFLAGS += -DLIBEXPORT=""

# List all source files in the project
SOURCES = tsnd_main.c

# Sources for dependencies and Silabs libraries
SOURCES += \
    $(ZOO)/thinnect.device-signature/signature/DeviceSignature.c \
    $(ZOO)/thinnect.device-signature/area/silabs/SignatureArea.c \
    $(SILABS_SDKDIR)/hardware/kit/common/drivers/retargetserial.c \
    $(SILABS_SDKDIR)/hardware/kit/common/drivers/retargetio.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_system.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_core.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_emu.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_cmu.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_rmu.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_gpio.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_usart.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_i2c.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_ldma.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_msc.c \
    $(SILABS_SDKDIR)/platform/emlib/src/em_timer.c \
    $(ZOO)/thinnect.node-platform/silabs/logger_fwrite.c \
    $(ZOO)/thinnect.lll/logging/loggers_ext.c \
    $(ZOO)/lammertb.libcrc/src/crcccitt.c \
    $(ZOO)/thinnect.node-platform/silabs/radio_rtos.c \
    $(ZOO)/thinnect.node-platform/common/radio_seqNum.c \
    $(wildcard $(ZOO)/thinnect.mist-comm/am/*.c) \
    $(wildcard $(ZOO)/thinnect.mist-comm/api/*.c) \
    $(RAIL_SRC) \
    $(MOTEXML_SRC) \
    $(FREERTOS_SRC)

# All include directories
INCLUDES += \
    -I$(BUILD_DIR) \
    -I. \
    -I$(ZOO)/graphitemaster.incbin \
    -I$(ZOO)/jtbr.endianness \
    -I$(ZOO)/thinnect.node-platform/include \
    -I$(ZOO)/thinnect.device-signature/signature/ \
    -I$(ZOO)/thinnect.device-signature/area/ \
    -I$(SILABS_SDKDIR)/hardware/kit/common/drivers \
    -I$(SILABS_SDKDIR)/platform/halconfig/inc/hal-config \
    -I$(SILABS_SDKDIR)/platform/emlib/inc \
    -I$(ZOO)/thinnect.lll/logging \
    -I$(ZOO)/thinnect.mist-comm/include -I$(ZOO)/thinnect.mist-comm/include/compat \
    -I$(ZOO)/lammertb.libcrc/include \
    $(RAIL_INC) \
    $(MOTEXML_INC) \
    $(FREERTOS_INC)

# ------------------------------------------------------------------------------

# Pull in the grunt work
include $(BUILDSYSTEM_DIR)/Makerules

$(call passVarToCpp,CFLAGS,VERSION_MAJOR)
$(call passVarToCpp,CFLAGS,VERSION_MINOR)
$(call passVarToCpp,CFLAGS,VERSION_PATCH)
$(call passVarToCpp,CFLAGS,VERSION_STR)
$(call passVarToCpp,CFLAGS,RADIO_CHANNEL)
$(call passVarToCpp,CFLAGS,DEFAULT_AM_ADDR)
$(call passVarToCpp,CFLAGS,DESTINATION_GATEWAY_ADDRESS)

# _______________________________ Project rules _______________________________

all: $(BUILD_DIR)/$(PROJECT_NAME).bin

# header.bin should be recreated if a build takes place
$(OBJECTS): $(BUILD_DIR)/header.bin

$(BUILD_DIR)/$(PROJECT_NAME).elf: Makefile | $(BUILD_DIR)

$(BUILD_DIR)/header.bin: Makefile | $(BUILD_DIR)
	$(call pInfo,Creating application header block [$@])
	$(HEADEREDIT) -c -v softtype,1 -v firmaddr,$(APP_START) -v firmsizemax,$(APP_MAX_LEN) \
	    -v version,$(VERSION_STR) -v versionbin,$(VERSION_BIN) \
	    -v uuid,$(UUID_BOARD) -v uuid2,$(UUID_PLATFORM) -v uuid3,$(UUID_APPLICATION) \
	    -v timestamp,$(BUILD_TIMESTAMP) \
	    -v name,$(PROJECT_NAME) \
	    -v size -v crc "$@"

$(BUILD_DIR)/$(PROJECT_NAME).elf: $(OBJECTS)
	$(call pInfo,Linking [$@])
	$(HIDE_CMD)$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) $(LDLIBS) $(LDFLAGS) -o $@

$(BUILD_DIR)/$(PROJECT_NAME).bin: $(BUILD_DIR)/$(PROJECT_NAME).elf
	$(call pInfo,Exporting [$@])
	$(HIDE_CMD)$(TC_SIZE) --format=Berkeley $<
	$(HIDE_CMD)$(TC_OBJCOPY) --strip-all -O binary "$<" "$@"
	$(HIDE_CMD)$(HEADEREDIT) -v size -v crc $@

$(PROJECT_NAME): $(BUILD_DIR)/$(PROJECT_NAME).bin

# _______________________________ Utility rules ________________________________

$(BUILD_DIR):
	$(call pInfo,Creating [$@])
	@mkdir -p "$@"

clean:
	$(call pInfo,Nuking everything in [$(BUILD_BASE_DIR)])
	@-rm -rf "$(BUILD_BASE_DIR)"

.PHONY: $(PHONY_GOALS)
