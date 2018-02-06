include $(MK)/prologue.mk

dir	:= $(d)/ke
include	$(dir)/Rules.mk
dir	:= $(d)/mm
include	$(dir)/Rules.mk
dir := $(d)/rf
include $(dir)/Rules.mk

include $(MK)/epilogue.mk