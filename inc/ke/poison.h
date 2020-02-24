#pragma once

#define POISON_BASE (0xDEAD000000000000)
#define POISON_STEP (0x1000)

#define MAKE_POISON(idx) (POISON_BASE + POISON_STEP * (idx))

#define POISON_LIST ((void*)MAKE_POISON(0))
#define POISON_AVL ((void*)MAKE_POISON(1))
