#ifndef MIST_DATA_MSG_H_
#define MIST_DATA_MSG_H_

/**
 * The fragmented data message structure.
 * The header specifies the message sequence number, the fragment number and
 * number of total fragments. The rest of the payload can be used for data.
 */
#pragma pack(push, 1)
typedef struct mist_frag_message
{
	uint8_t seq;
	uint8_t frags: 4; // Comes second, but specified first because of endianness
	uint8_t frag: 4;  // Comes first, but specified second because of endianness
	uint8_t data[];
} mist_frag_message_t;
#pragma pack(pop)

#define AMID_MIST_DATA 0xE1

#endif//MIST_DATA_MSG_H_
