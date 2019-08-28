/**
 * The RadioCountToLeds application for Silabs Wireless Gecko platforms.
 *
 * Configure platform, logging to UART, start kernel and start sending
 * / receiving count messages, displaying received count on LEDs.
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

#include "radio_count_to_leds.h"

#include "DeviceSignature.h"
#include "mist_comm_am.h"
#include "radio.h"

#include "endianness.h"

#include "loglevels.h"
#define __MODUUL__ "main"
#define __LOG_LEVEL__ (LOG_LEVEL_main & BASE_LOG_LEVEL)
#include "log.h"

// Include the information header binary
#include "incbin.h"
INCBIN(Header, "header.bin");

static comms_msg_t m_msg;
static bool m_sending = false;
static osMutexId_t m_mutex;

static void receive_message (comms_layer_t* comms, const comms_msg_t* msg, void* user)
{
	if (comms_get_payload_length(comms, msg) >= sizeof(radio_count_msg_t))
	{
		radio_count_msg_t * packet = (radio_count_msg_t*)comms_get_payload(comms, msg, sizeof(radio_count_msg_t));
		uint16_t counter = ntoh16(packet->counter);
		info1("rcv:%u", (unsigned int)counter);
		PLATFORM_LedsSet((uint8_t)counter);
	}
	else warn1("size %d", (unsigned int)comms_get_payload_length(comms, msg));
}

static void radio_send_done (comms_layer_t * comms, comms_msg_t * msg, comms_error_t result, void * user)
{
	logger(result == COMMS_SUCCESS ? LOG_DEBUG1: LOG_WARN1, "snt %u", result);
	while(osMutexAcquire(m_mutex, 1000) != osOK);
	m_sending = false;
	osMutexRelease(m_mutex);
}

static comms_layer_t* radio_setup (am_addr_t node_addr)
{
	static comms_receiver_t rcvr;
	comms_layer_t * radio = radio_init(RADIO_CHANNEL, 0x22, node_addr);
	if (NULL != radio)
	{
		comms_register_recv(radio, &rcvr, receive_message, NULL, AMID_RADIO_COUNT_TO_LEDS);
		debug1("radio rdy");
	}
	return radio;
}

// App loop - increment and send counter
void app_loop ()
{
	uint16_t counter = 0;
	am_addr_t node_addr;
	uint8_t node_eui[8];

	m_mutex = osMutexNew(NULL);

	// Initialize node signature - get address and EUI64
	sigInit();
	node_addr = sigGetNodeId();
	sigGetEui64(node_eui);
	infob1("ADDR:%"PRIX16" EUI64:", node_eui, sizeof(node_eui), node_addr);

	// initialize radio
	comms_layer_t* radio = radio_setup(node_addr);
	if (NULL == radio)
	{
		err1("radio");
		for (;;); // panic
	}

	for (;;)
	{
		info1("ctr:%u", (unsigned int)counter);

		while(osMutexAcquire(m_mutex, 1000) != osOK);
		if(false == m_sending)
		{
			comms_init_message(radio, &m_msg);
        	radio_count_msg_t * packet = comms_get_payload(radio, &m_msg, sizeof(radio_count_msg_t));
            if (NULL != packet)
            {
            	packet->counter = hton16(counter);

	            // Send data packet
	            comms_set_packet_type(radio, &m_msg, AMID_RADIO_COUNT_TO_LEDS);
	            comms_am_set_destination(radio, &m_msg, AM_BROADCAST_ADDR);
	            //comms_am_set_source(radio, &m_msg, radio_address); // No need, it will use default
	            comms_set_payload_length(radio, &m_msg, sizeof(radio_count_msg_t));

	            comms_error_t result = comms_send(radio, &m_msg, radio_send_done, NULL);
	            logger(result == COMMS_SUCCESS ? LOG_DEBUG1: LOG_WARN1, "snd %u", result);
	            if (COMMS_SUCCESS == result)
	            {
	                m_sending = true;
	            }
	        }
	        else err1("pckt");
		}
		osMutexRelease(m_mutex);

		osDelay(250);
		counter++;
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

	info1("RadioCountToLeds "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

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
