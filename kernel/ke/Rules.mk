include $(MK)/prologue.mk

SRC_$(d) := $(d)/alloc.c \
			$(d)/assert.c \
			$(d)/atomic.c \
			$(d)/boot.c \
			$(d)/bug_check.c \
			$(d)/intr.c \
			$(d)/print.c \
			$(d)/rwwlock.c \
			$(d)/spin_lock.c
			
include $(MK)/stdrules.mk

include $(MK)/epilogue.mk