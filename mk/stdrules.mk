# The source rules.mk defines:
# SRC_$(d) for all c source files
# SRCAS_$(d) for all asm source files
# SRCIN_$(d) for all in(preprocessor) source files
# Compiles all c and in source files and generate dependencies
# Adds c and asm object files to $OBJ variable
# Adds all generated files to $CLEAN variable

OBJ_$(d) := $(OBJ_$(d)) $(addprefix $(OUT)/, $(SRC_$(d):.c=.o))
OBJAS_$(d) := $(OBJAS_$(d)) $(addprefix $(OUT)/, $(SRCAS_$(d):.asm=.a))
OBJIN_$(d) := $(OBJIN_$(d)) $(addprefix $(OUT)/, $(SRCIN_$(d):.in=))
DEP_$(d) := $(DEP_$(d)) $(addsuffix .d, $(OBJ_$(d)) $(OBJIN_$(d)))

$(OBJ_$(d)): $(OUT)/%.o: %.c
	$(MKDIR)
	$(COMP)
	$(GDEP)

$(OBJAS_$(d)): $(OUT)/%.a: %.asm
	$(MKDIR)
	$(COMPAS)

$(OBJIN_$(d)): $(OUT)/%: %.in
	$(MKDIR)
	$(PREP)
	$(GDEP)

# append all OBJECTS to OBJ
OBJ := $(OBJ) $(OBJ_$(d)) $(OBJAS_$(d))
CLEAN := $(CLEAN) $(OBJ_$(d)) $(OBJAS_$(d)) $(OBJIN_$(d)) $(DEP_$(d))

-include $(DEP_$(d))