include $(MK)/prologue.mk

SRC_$(d) := $(d)/alloc.c \
			$(d)/assert.c \
			$(d)/atomic.c \
			$(d)/panic.c \
			$(d)/intr.c \
			$(d)/print.c \
			$(d)/rww_lock.c \
			$(d)/spin_lock.c \
			$(d)/main.c
			
include $(MK)/stdrules.mk

include $(MK)/epilogue.mk