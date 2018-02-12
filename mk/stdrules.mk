# The source rules.mk defines:
# SRC_$(d) for all c source files
# SRCAS_$(d) for all asm source files
# SRCIN_$(d) for all in(preprocessor) source files
# Compiles all c and in source files and generate dependencies
# Adds c and asm object files to $OBJ variable
# Adds all generated files to $CLEAN variable

OBJ_$(d) := $(SRC_$(d):.c=.o)
OBJAS_$(d) := $(SRCAS_$(d):.asm=.a)
OBJIN_$(d) := $(SRCIN_$(d):.in=)
DEP_$(d) := $(SRC_$(d):.c=.d) $(SRCIN_$(d):.in=.d)

$(OBJ_$(d)): %.o: %.c
	$(COMP)
	$(GDEP)

$(OBJAS_$(d)): %.a: %.asm 
	$(COMPAS)

$(OBJIN_$(d)): %: %.in
	$(PREP)
	$(GDEP)

# append all OBJECTS to OBJ
OBJ := $(OBJ) $(OBJ_$(d)) $(OBJAS_$(d))
CLEAN := $(CLEAN) $(OBJ_$(d)) $(OBJAS_$(d)) $(OBJIN_$(d)) $(DEP_$(d))

include $(DEP_$(d))