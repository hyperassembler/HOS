include $(MK)/prologue.mk

MOD:=KERNEL
C_FLAGS_$(MOD):=$(addprefix -I, $(d)/inc)
AS_FLAGS_$(MOD):=$(addprefix -I, $(d)/inc)

dir	:= $(d)/ke
include	$(dir)/Rules.mk
dir	:= $(d)/mm
include	$(dir)/Rules.mk
dir := $(d)/rf
include $(dir)/Rules.mk
dir := $(d)/lb
include $(dir)/Rules.mk

include $(MK)/epilogue.mk