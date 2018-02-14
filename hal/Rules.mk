include $(MK)/prologue.mk

SRC_$(d) := $(d)/boot.c \
			$(d)/intr.c \
			$(d)/mem.c \
			$(d)/print.c

SRCAS_$(d) := $(d)/cpu.asm \
			  $(d)/intr.asm

SRCIN_$(d) := $(d)/boot.asm.in

#special rules for boot.asm
#no dependencies for ASM objects
$(OUT)/$(d)/boot.a: $(OUT)/$(d)/boot.asm
	$(MKDIR)
	$(COMPAS)

OBJ := $(OBJ) $(OUT)/$(d)/boot.a
CLEAN := $(CLEAN) $(OUT)/$(d)/boot.a

# include this at last
include $(MK)/stdrules.mk

include $(MK)/epilogue.mk