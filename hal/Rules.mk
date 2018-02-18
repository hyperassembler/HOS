include $(MK)/prologue.mk

SRC_$(d) := $(d)/boot.c \
			$(d)/intr.c \
			$(d)/mem.c \
			$(d)/print.c

SRCAS_$(d) := $(d)/cpu.asm \
			  $(d)/intr.asm

SRCIN_$(d) := $(d)/boot.asm.in

# include this at last
include $(MK)/stdrules.mk

include $(MK)/epilogue.mk