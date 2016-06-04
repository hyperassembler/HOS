ASM = nasm
CC = gcc
LD = ld

#Recursive Wildcard
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
rdircard=$(sort $(dir $(call rwildcard,$1,*)))
rdircardex=$(sort $(dir $(call rwildcard,$1,$2)))

#x64 vars
LD_SCRIPT := linker.ld
GRUB_CFG := grub.cfg

OUTPUT_DIR := out
HEADER_DIRS := $(call rdircardex, *,*.h)
ALL_OUTPUT_DIRS := $(addprefix $(OUTPUT_DIR)/,$(call rdircard,*))

C_FLAGS := -m64 -std=c11 -g -c $(addprefix -I, $(HEADER_DIRS)) -fno-stack-protector -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -masm=intel -Wall -Wextra -Wno-comment
ASM_FLAGS := -f elf64 -I $(ASM_SRC_PATH_64)/
LD_FLAGS := -melf_x86_64

C_SRC := $(call rwildcard, ./, *.c)
ASM_SRC := $(call rwildcard, ./, *.asm)

KERNEL_BIN := kernel.bin
KERNEL_ELF := kernel.elf

#Object files
C_OBJ := $(C_SRC:.c=.o)
ASM_OBJ := $(ASM_SRC:.asm=.oasm)
ALL_OBJ := $(addprefix $(OUTPUT_DIR)/,$(C_OBJ)) $(addprefix $(OUTPUT_DIR)/,$(ASM_OBJ))


#Commands
all: init compile link buildiso clean

init:
	mkdir -p $(ALL_OUTPUT_DIRS)

compile: $(C_OBJ) $(ASM_OBJ)

link: $(KERNEL_BIN)

buildiso:
	mkdir -p $(OUTPUT_DIR)/temp_iso/secX
	mkdir -p $(OUTPUT_DIR)/temp_iso/boot
	mkdir -p $(OUTPUT_DIR)/temp_iso/boot/grub
	mv $(OUTPUT_DIR)/$(KERNEL_BIN) $(OUTPUT_DIR)/temp_iso/secX/$(KERNEL_BIN)
	cp $(GRUB_CFG) $(OUTPUT_DIR)/temp_iso/boot/grub/
	grub-mkrescue -o secX.iso $(OUTPUT_DIR)/temp_iso
	rm -rf $(OUTPUT_DIR)/temp_iso

clean:
	rm -rf $(OUTPUT_DIR)

%.o: %.c
	$(CC) $(C_FLAGS) -o $(OUTPUT_DIR)/$@ $^

%.oasm: %.asm
	$(ASM) $(ASM_FLAGS) -o $(OUTPUT_DIR)/$@ $^

$(KERNEL_BIN): $(ALL_OBJ)
	$(LD) $(LD_FLAGS) -T $(LD_SCRIPT) -o $(OUTPUT_DIR)/$(KERNEL_BIN) $(ALL_OBJ)

