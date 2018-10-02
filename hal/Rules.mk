include $(MK)/prologue.mk

MOD:=HAL
C_FLAGS_$(MOD):=$(addprefix -I, $(d)/inc)
AS_FLAGS_$(MOD):=$(addprefix -I, $(d)/inc)

SRC_$(d) := $(d)/boot.c \
			$(d)/intr.c \
			$(d)/mem.c \
			$(d)/print.c \
			$(d)/hal.c

SRCAS_$(d) := $(d)/cpu.asm \
			  $(d)/intr.asm \
			  $(d)/io.asm \
			  $(d)/atomic.asm

SRCIN_$(d) := $(d)/boot.asm.in \
			  $(d)/mb_hdr.asm.in

#special rules for preprocessed asm files
$(OUT)/$(d)/mb_hdr.a: $(OUT)/$(d)/mb_hdr.asm
	$(MKDIR)
	$(COMPAS)

$(OUT)/$(d)/boot.a: $(OUT)/$(d)/boot.asm
	$(MKDIR)
	$(COMPAS)

OBJ := $(OBJ) $(OUT)/$(d)/boot.a $(OUT)/$(d)/mb_hdr.a
CLEAN := $(CLEAN) $(OUT)/$(d)/boot.a $(OUT)/$(d)/mb_hdr.a

# include this at last
include $(MK)/stdrules.mk

include $(MK)/epilogue.mk