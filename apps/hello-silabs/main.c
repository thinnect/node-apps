/**
 * HelloWorld!
 *
 * Copyright Thinnect Inc. 2019
 * @license MIT
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "retargetserial.h"

#include "platform.h"

// Include the information header binary
#include "incbin.h"
INCBIN(Header, "header.bin");

int main ()
{
    PLATFORM_Init();

    RETARGET_SerialInit();

    printf("HelloWorld!\n");

    for(;;);
}
