#ifndef _HAL_IO_H_
#define _HAL_IO_H_
#include "../common/kdef.h"
#include "../common/type.h"
extern void NATIVE64 hal_write_port(uint64_t port, int64_t data);
extern int64_t NATIVE64 hal_read_port(uint64_t port);
#endif