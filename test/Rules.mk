include $(MK)/prologue.mk

SRC_$(d) := $(d)/avl_tree_test.c \
			$(d)/driver.c \
			$(d)/linked_list_test.c \
			$(d)/salloc_test.c

include $(MK)/stdrules.mk

include $(MK)/epilogue.mk