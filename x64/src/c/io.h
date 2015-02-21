#ifndef _IO_H_
#define _IO_H_
#include "kdef.h"
#include "type.h"
extern void NATIVE64 hk_write_port(uint64_t port, int64_t data);
extern int64_t NATIVE64 hk_read_port(uint64_t port);
#endif