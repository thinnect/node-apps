/**
 * Basic Blink app for Silabs Wireless Gecko platforms.
 *
 * Configure platform, logging to UART, start kernel and keep blinking LEDs.
 *
 * Copyright Thinnect Inc. 2019
 * @license MIT
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "em_chip.h"
#include "em_rmu.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_msc.h"
#include "em_i2c.h"
#include "em_adc.h"
#include "em_usart.h"

#include "retargetserial.h"

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

// App loop - blinks leds
void app_loop ()
{
	uint8_t leds = 0;
	for (;;)
	{
		PLATFORM_LedsSet(leds);
		info1("leds %u", (unsigned int)leds);
		osDelay(1000);
		leds++;
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

	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_PRS, true);

	// LEDs
	PLATFORM_LedsInit();

	// Configure debug output
	RETARGET_SerialInit();
	log_init(BASE_LOG_LEVEL, &logger_fwrite_boot, NULL);

	info1("Blink "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

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
