#ifndef _MEM_32_H_
#define _MEM_32_H_

#include "../hdr/io32.h"

#pragma push()
#pragma pack(1)
typedef struct
{
    UINT16 LimitLow;
    UINT16 BaseLow;
    UINT8 BaseMid;
    UINT8 AccessByte;
    UINT8 FlagLimitMid;
    UINT8 BaseHigh;
} GDT_DESCRIPTOR;
#pragma pop()


#endif