include $(MK)/prologue.mk

SRCIN_$(d) := $(d)/linker.ld.in \
			  $(d)/grub.cfg.in

include $(MK)/stdrules.mk

include $(MK)/epilogue.mk