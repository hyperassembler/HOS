include $(MK)/prologue.mk

SRC_$(d) := $(d)/avl_tree.c \
			$(d)/linked_list.c \
			$(d)/salloc.c \
			$(d)/sxtdlib.c

include $(MK)/stdrules.mk

include $(MK)/epilogue.mk