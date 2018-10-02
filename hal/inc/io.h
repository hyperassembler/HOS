#pragma once

#include "hdef.h"

/**
 * ASM declarations
 */

int8 HABI hal_read_port_8(uint16 port);

int16 HABI hal_read_port_16(uint16 port);

int32 HABI hal_read_port_32(uint16 port);

void HABI hal_write_port_8(uint16 port, uint8 data);

void HABI hal_write_port_16(uint16 port, uint16 data);

void HABI hal_write_port_32(uint16 port, uint32 data);
