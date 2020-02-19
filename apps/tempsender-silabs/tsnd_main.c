/**
 * The TempSender application for Silabs Wireless Gecko platforms.
 *
 * Configure platform, logging to UART, start kernel and start sending messages.
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

#include "DeviceSignature.h"
#include "mist_comm_am.h"
#include "radio.h"

#include "endianness.h"

#include "mist_data_msg.h"
#include "ML.h"
#include "MLE.h"
#include "dt_types.h"

#include "loglevels.h"
#define __MODUUL__ "main"
#define __LOG_LEVEL__ (LOG_LEVEL_main & BASE_LOG_LEVEL)
#include "log.h"

// Include the information header binary
#include "incbin.h"
INCBIN(Header, "header.bin");

/**
 * Fixed destination address specified at compile-time.
 */
#ifndef DESTINATION_GATEWAY_ADDRESS
#warning Using BROADCAST address for DESTINATION_GATEWAY_ADDRESS
#define DESTINATION_GATEWAY_ADDRESS 0xFFFF
#endif//DESTINATION_GATEWAY_ADDRESS

static comms_msg_t m_msg;
static bool m_sending = false;
static osMutexId_t m_mutex;

// Message has been sent
static void radio_send_done (comms_layer_t * comms, comms_msg_t * msg, comms_error_t result, void * user)
{
    logger(result == COMMS_SUCCESS ? LOG_DEBUG1: LOG_WARN1, "snt %u", result);
    osMutexAcquire(m_mutex, osWaitForever);
    m_sending = false;
    osMutexRelease(m_mutex);
    PLATFORM_LedsSet(0);
}

static void radio_start_done (comms_layer_t * comms, comms_status_t status, void * user)
{
    debug("started %d", status);
}

// Perform basic radio setup, register to receive RadioCountToLeds packets
static comms_layer_t* radio_setup (am_addr_t node_addr)
{
    comms_layer_t * radio = radio_init(DEFAULT_RADIO_CHANNEL, 0x22, node_addr);
    if (NULL == radio)
    {
        return NULL;
    }

    if (COMMS_SUCCESS != comms_start(radio, radio_start_done, NULL))
    {
        return NULL;
    }

    // Wait for radio to start, could use osTreadFlagWait and set from callback
    while(COMMS_STARTED != comms_status(radio))
    {
        osDelay(1);
    }
    debug1("radio rdy");
    return radio;
}

uint8_t encode_temperature_payload(void* payload, uint8_t length,
                                   float value,
                                   uint32_t seq,
                                   uint8_t eui[8], uint8_t resource_feature[16])
{
    ml_encoder_t enc;
    if(ML_SUCCESS == MLE_initialize(&enc, payload, length))
    {
        uint8_t index1, index2, index3;
        index1 = MLE_appendO(&enc, dt_data);

        // Add the temperature element (there are different types of temperatures)
        index2 = MLE_appendOSV(&enc, dt_data, index1, dt_ambient_temperature_C);
            index3 = MLE_appendOSV(&enc, dt_value, index2, value*10);
                MLE_appendOSV(&enc, dt_exp, index3, -1);

        // Add an UTC timestamp with the epoc at 2000-01-01
        //  index2 = MLE_appendOSV(&enc, dt_timestamp_utc, index1, 604401880);

        index1 = MLE_appendOV(&enc, dt_subscription, 2);
            index2 = MLE_appendOSB(&enc, dt_provider, index1, eui, 8);
            index2 = MLE_appendOSV(&enc, dt_seq, index1, seq);
            index2 = MLE_appendOSB(&enc, dt_resource, index1, resource_feature, 16);
        return MLE_finalize(&enc);
    }
    return 0;
}

// App loop - send data
void app_loop ()
{
    uint32_t seqnum = 0;
    am_addr_t node_addr = DEFAULT_AM_ADDR;
    // UUID identifying a temperature-sensor resource
    const char resource_feature[] = "\xf0\x63\xc7\x51\xa8\xd4\x46\x73\xaf\x38\x6d\xf8\x2a\x9b\xb9\x42";
    uint8_t node_eui[8];
    float temperature = 22.5; // Fake temperature - you should really get this from an actual sensor

    m_mutex = osMutexNew(NULL);

    // Initialize node signature - get address and EUI64
    if (SIG_GOOD == sigInit())
    {
        node_addr = sigGetNodeId();
        sigGetEui64(node_eui);
        infob1("ADDR:%"PRIX16" EUI64:", node_eui, sizeof(node_eui), node_addr);
    }
    else
    {
        warn1("ADDR:%"PRIX16, node_addr); // Falling back to default addr
        memset(node_eui, 0, sizeof(node_eui));
        node_eui[6] = (uint8_t)(node_addr >> 8);
        node_eui[7] = (uint8_t)node_addr;
    }
    debugb1("EUI:", node_eui, sizeof(node_eui));

    // initialize radio
    comms_layer_t* radio = radio_setup(node_addr);
    if (NULL == radio)
    {
        err1("radio");
        for (;;); // panic
    }

    // Lopp forever, incrementing counter and broadcasting it
    for (;;)
    {
        info1("snd:%"PRIu32, seqnum);

        osMutexAcquire(m_mutex, osWaitForever);
        if(false == m_sending)
        {
            uint8_t max_len = comms_get_payload_max_length(radio);

            // While the data format supports fragmentation, we do not use it in
            // this example - size must be kept small to fit a single fragment.

            comms_init_message(radio, &m_msg);
            mist_frag_message_t * packet = comms_get_payload(radio, &m_msg, max_len);
            if ((NULL != packet)&&(max_len > sizeof(mist_frag_message_t)))
            {
                uint8_t len = sizeof(mist_frag_message_t);

                packet->seq = (uint8_t)seqnum;
                packet->frag = 0;
                packet->frags = 1;

                len += encode_temperature_payload(packet->data, max_len - len,
                                                  temperature, seqnum,
                                                  node_eui, (uint8_t*)resource_feature);

                // Send data packet
                comms_set_packet_type(radio, &m_msg, AMID_MIST_DATA);
                comms_am_set_destination(radio, &m_msg, DESTINATION_GATEWAY_ADDRESS);
                //comms_am_set_source(radio, &m_msg, radio_address); // No need, it will use the one set with radio_init
                comms_set_payload_length(radio, &m_msg, len);

                comms_error_t result = comms_send(radio, &m_msg, radio_send_done, NULL);
                logger(result == COMMS_SUCCESS ? LOG_DEBUG1: LOG_WARN1, "snd(%u) %u", (unsigned int)len, result);
                if (COMMS_SUCCESS == result)
                {
                    m_sending = true;
                    PLATFORM_LedsSet(1);
                }
            }
            else err1("pckt");
        }
        osMutexRelease(m_mutex);

        osDelay(30000);
        seqnum++;
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

    info1("TempSender "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    PLATFORM_RadioInit(); // Radio GPIO/PRS - LNA on some MGM12P

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
