#ifndef PLATFORM_MSG_H_
#define PLATFORM_MSG_H_

#include "mist_comm_am_msg.h"

#define COMMS_MSG_ADDRESSING_SIZE 2
#define COMMS_MSG_HEADER_SIZE 1
#define COMMS_MSG_PAYLOAD_SIZE 128
#define COMMS_MSG_FOOTER_SIZE 0
#define COMMS_MSG_METADATA_SIZE sizeof(comms_am_msg_metadata_t)

#endif//PLATFORM_MSG_H_
