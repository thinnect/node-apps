/**
 * Read the JEDEC-ID from an external dataflash on SPI.
 *
 * Configure platform, logging to UART, start kernel and read the dataflash ID.
 *
 * Copyright Thinnect Inc. 2019
 * @license MIT
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "retargetserial.h"
#include "retargetspi.h"

#include "cmsis_os2.h"

#include "platform.h"

#include "SignatureArea.h"
#include "DeviceSignature.h"

#include "loggers_ext.h"
#include "logger_fwrite.h"

#include "loglevels.h"
#define __MODUUL__ "main"
#define __LOG_LEVEL__ (LOG_LEVEL_main & BASE_LOG_LEVEL)
#include "log.h"

// Include the information header binary
#include "incbin.h"
INCBIN(Header, "header.bin");

// App loop - do setup and periodically print status
void app_loop ()
{
    osDelay(1000);

    for (;;)
    {
	    // Enable SPI for dataflash
	    RETARGET_SpiInit();

	    // Wake dataflash from deep sleep
	    RETARGET_SpiTransferHalf(0, "\xAB", 1, NULL, 0);
	    osDelay(50);

	    // Get dataflash chip ID
	    uint8_t jedec[4] = {0};
	    RETARGET_SpiTransferHalf(0, "\x9F", 1, jedec, 4);
	    info1("jedec %02x%02x%02x%02x", jedec[0], jedec[1], jedec[2], jedec[3]);
        // C22814?? MX25R8035F
        // 016017?? IS25LP064D

		// Put dataflash into deep sleep
		RETARGET_SpiTransferHalf(0, "\xB9", 1, NULL, 0);

		// Disable SPI
	    RETARGET_SpiDisable();

	    // Wait a while
	    osDelay(5000);
    }
}

int logger_fwrite_boot (const char *ptr, int len)
{
    fwrite(ptr, len, 1, stdout);
    fflush(stdout);
    return len;
}

int main ()
{
    PLATFORM_Init();

    // LEDs
    PLATFORM_LedsInit();
    PLATFORM_ButtonPinInit();

    // Configure debug output
    RETARGET_SerialInit();
    log_init(BASE_LOG_LEVEL, &logger_fwrite_boot, NULL);

    info1("Dataflash "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    // Initialize OS kernel
    osKernelInitialize();

    // Create a thread
    const osThreadAttr_t app_thread_attr = { .name = "app" };
    osThreadNew(app_loop, NULL, &app_thread_attr);

    if (osKernelReady == osKernelGetState())
    {
        // Switch to a thread-safe logger
        logger_fwrite_init();
        log_init(BASE_LOG_LEVEL, &logger_fwrite, NULL);

        // Start the kernel
        osKernelStart();
    }
    else
    {
        err1("!osKernelReady");
    }

    for(;;);
}
