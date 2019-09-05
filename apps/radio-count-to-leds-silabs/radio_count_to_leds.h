#ifndef RADIO_COUNT_TO_LEDS_H_
#define RADIO_COUNT_TO_LEDS_H_

#pragma pack(push, 1)
typedef struct radio_count_msg
{
    uint16_t counter; // BigEndian
} radio_count_msg_t;
#pragma pack(pop)

enum RadioCountToLedsEnum
{
    AMID_RADIO_COUNT_TO_LEDS = 6,
};

#endif//RADIO_COUNT_TO_LEDS_H_
