#ifndef _IO_H_
#define _IO_H_
#include "../common/kdef.h"
#include "../common/type.h"
extern void NATIVE64 write_port(uint64_t port, int64_t data);
extern int64_t NATIVE64 read_port(uint64_t port);
#endif