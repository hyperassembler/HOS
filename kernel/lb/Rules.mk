include $(MK)/prologue.mk

SRC_$(d) := $(d)/atree.c \
			$(d)/llist.c \
			$(d)/salloc.c
include $(MK)/stdrules.mk

include $(MK)/epilogue.mk