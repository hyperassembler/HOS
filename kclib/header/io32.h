#ifndef _IO32_H_
#define _IO32_H_

#include "type32.h"

extern void HkWritePort(uint16 port, uint16 data);
extern void HkReadPort(uint16 port, uint16 data);

#endif