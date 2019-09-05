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

static osMutexId_t m_led_mutex;

static void led0_timer_cb(void* argument)
{
    osMutexAcquire(m_led_mutex, osWaitForever);
    debug1("led0 timer");
    PLATFORM_LedsSet(PLATFORM_LedsGet()^1);
    osMutexRelease(m_led_mutex);
}

static void led1_timer_cb(void* argument)
{
    osMutexAcquire(m_led_mutex, osWaitForever);
    debug1("led1 timer");
    PLATFORM_LedsSet(PLATFORM_LedsGet()^2);
    osMutexRelease(m_led_mutex);
}

static void led2_timer_cb(void* argument)
{
    osMutexAcquire(m_led_mutex, osWaitForever);
    debug1("led2 timer");
    PLATFORM_LedsSet(PLATFORM_LedsGet()^4);
    osMutexRelease(m_led_mutex);
}

// App loop - do setup and periodically print status
void app_loop ()
{
    m_led_mutex = osMutexNew(NULL);

    osDelay(1000);

    osTimerId_t led0_timer = osTimerNew(&led0_timer_cb, osTimerPeriodic, NULL, NULL);
    osTimerId_t led1_timer = osTimerNew(&led1_timer_cb, osTimerPeriodic, NULL, NULL);
    osTimerId_t led2_timer = osTimerNew(&led2_timer_cb, osTimerPeriodic, NULL, NULL);

    debug1("t1 %p t2 %p t3 %p", led0_timer, led1_timer, led2_timer);

    osTimerStart(led0_timer, 1000);
    osTimerStart(led1_timer, 2000);
    osTimerStart(led2_timer, 4000);

    for (;;)
    {
        osMutexAcquire(m_led_mutex, osWaitForever);
        info1("leds %u", (unsigned int)PLATFORM_LedsGet());
        osMutexRelease(m_led_mutex);
        osDelay(1000);
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
