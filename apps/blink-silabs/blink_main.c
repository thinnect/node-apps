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

#include "retargetserial.h"

#include "cmsis_os2.h"

#include "platform.h"

#include "SignatureArea.h"
#include "DeviceSignature.h"

#include "basic_rtos_logger_setup.h"

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
    // Switch to a thread-safe logger
    basic_rtos_logger_setup();

    m_led_mutex = osMutexNew(NULL);

    osDelay(1000);

    osTimerId_t led0_timer = osTimerNew(&led0_timer_cb, osTimerPeriodic, NULL, NULL);
    osTimerId_t led1_timer = osTimerNew(&led1_timer_cb, osTimerPeriodic, NULL, NULL);
    osTimerId_t led2_timer = osTimerNew(&led2_timer_cb, osTimerPeriodic, NULL, NULL);

    debug1("t1 %p t2 %p t3 %p", led0_timer, led1_timer, led2_timer);

    PLATFORM_LedsSet(0);

    osTimerStart(led0_timer, 1000);
    osTimerStart(led1_timer, 2000);
    osTimerStart(led2_timer, 4000);

    for (;;)
    {
        osMutexAcquire(m_led_mutex, osWaitForever);
        info1("leds %u", (unsigned int)PLATFORM_LedsGet());
        osMutexRelease(m_led_mutex);
        osDelay(1000);
        if(PLATFORM_ButtonGet())
        {
            info1("BUTTON");
        }
    }
}

int main ()
{
    PLATFORM_Init();

    // LEDs
    PLATFORM_LedsInit();
    PLATFORM_ButtonPinInit();

    // Configure debug output
    basic_noos_logger_setup();

    info1("Blink "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    // Initialize OS kernel
    osKernelInitialize();

    // Create a thread
    const osThreadAttr_t app_thread_attr = { .name = "app" };
    osThreadNew(app_loop, NULL, &app_thread_attr);

    if (osKernelReady == osKernelGetState())
    {
        osKernelStart();
    }
    else
    {
        err1("!osKernelReady");
    }

    for(;;);
}
