include $(MK)/prologue.mk

SRC_$(d) := init.c \
			intr.c \
			mem.c \
			print.c 

SRC_$(d) := $(addprefix $(d)/, $(SRC_$(d)))

OBJ_$(d) := $(SRC_$(d):.c=.o)

SRCAS_$(d) := boot.asm \
			  cpu.asm \
			  intr.asm 

OBJAS_$(d) := $(SRCAS_$(d):.c=.a)

$(OBJ_$(d)): %.o: %.c
	$(COMP)

$(OBJAS_$(d)): %.a: %.asm 
	$(COMPAS)

# append all OBJECTS to clean
OBJ := $(OBJ) $(OBJ_$(d)) $(OBJAS_$(d))

include $(MK)/epilogue.mk