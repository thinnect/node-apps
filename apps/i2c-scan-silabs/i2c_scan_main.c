/**
 * Scan the I2C bus for devices.
 *
 * Configure platform, logging to UART, start kernel and scan I2C.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "retargetserial.h"
#include "retargeti2c.h"

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

#include "em_usart.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_rmu.h"

// App loop - do setup and periodically print status
void app_loop ()
{
    osDelay(1000);

    for (;;)
    {
	    // Enable I2C
	    RETARGET_I2CInit();

	    for(uint8_t addr=0; addr<128; addr++)
	    {
	    	uint8_t data[1];
	    	if (0 == RETARGET_I2CRead(addr, 0, data, 1))
	    	{
	    		// Print 7-bit addr, read addr, write addr
	    		info1("%02"PRIx8" %02"PRIx8"/%02"PRIx8, addr, (addr << 1) | 1, (addr << 1));
	    	}
	    	else
	    	{
	    		debug1("%02"PRIx8" -", addr);
	    	}
	    }

	    RETARGET_I2CDeinit();

	    // Wait a while
	    osDelay(10000);
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

    info1("I2C scan "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

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
