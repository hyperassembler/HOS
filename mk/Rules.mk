include $(MK)/prologue.mk

MOD:=MK

SRCIN_$(d) := $(d)/linker.ld.in

include $(MK)/stdrules.mk

include $(MK)/epilogue.mk