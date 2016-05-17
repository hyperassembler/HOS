#ifndef _STR_H_
#define _STR_H_

#include "../../sys/kdef.h"
#include "../../sys/type.h"

uint64_t SAPI str_len(char const *str);
uint64_t SAPI str_cmp(char const *str1, char const *str2);

#endif