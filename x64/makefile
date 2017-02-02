ASM = nasm
CC = gcc
LD = ld

#Recursive Wildcard
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
rdircard=$(sort $(dir $(call rwildcard,$1,*)))
rdircardex=$(sort $(dir $(call rwildcard,$1,$2)))

#x64 vars
LD_SCRIPT := build/linker.ld
GRUB_CFG := build/grub.cfg

TEMP_DIR := tmp
HEADER_DIRS := $(call rdircardex, *,*.h)
ALL_TEMP_DIRS := $(addprefix $(TEMP_DIR)/,$(call rdircard,*))

C_FLAGS := -m64 -std=c11 -g -c $(addprefix -I, $(HEADER_DIRS)) -mcmodel=kernel -ffreestanding -nostdlib -lgcc -fno-stack-protector -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -masm=intel -Wall -Werror -Wextra -Wno-comment
ASM_FLAGS := -f elf64 -g -I $(ASM_SRC_PATH_64)/
LD_FLAGS := -melf_x86_64 -I

C_SRC := $(call rwildcard, ./, *.c)
ASM_SRC := $(call rwildcard, ./, *.asm)

KERNEL_ELF := kernel.elf
KERNEL_DASM := kernel.dasm

#Object files
C_OBJ := $(C_SRC:.c=.o)
ASM_OBJ := $(ASM_SRC:.asm=.oasm)
ALL_OBJ := $(addprefix $(TEMP_DIR)/,$(C_OBJ)) $(addprefix $(TEMP_DIR)/,$(ASM_OBJ))


#Commands
all: init compile link buildiso disasm clean_temp

init:
	mkdir -p $(ALL_TEMP_DIRS)

compile: $(C_OBJ) $(ASM_OBJ)

link: $(KERNEL_ELF)

buildiso:
	mkdir -p $(TEMP_DIR)/temp_iso/secX
	mkdir -p $(TEMP_DIR)/temp_iso/boot
	mkdir -p $(TEMP_DIR)/temp_iso/boot/grub
	cp $(TEMP_DIR)/$(KERNEL_ELF) $(TEMP_DIR)/temp_iso/secX/$(KERNEL_ELF)
	cp $(TEMP_DIR)/$(KERNEL_ELF) ./
	cp $(GRUB_CFG) $(TEMP_DIR)/temp_iso/boot/grub/
	grub-mkrescue -o secX.iso $(TEMP_DIR)/temp_iso

clean_temp:
	rm -rf $(TEMP_DIR)

clean: clean_temp
	rm -rf $(KERNEL_DASM)
	rm -rf secX.iso
	rm -rf *.log
	rm -rf $(KERNEL_ELF)

%.o: %.c
	$(CC) $(C_FLAGS) -o $(TEMP_DIR)/$@ $^

%.oasm: %.asm
	$(ASM) $(ASM_FLAGS) -o $(TEMP_DIR)/$@ $^

$(KERNEL_ELF): $(ALL_OBJ)
	$(LD) $(LD_FLAGS) -T $(LD_SCRIPT) -o $(TEMP_DIR)/$(KERNEL_ELF) $(ALL_OBJ)

disasm:
	objdump -M intel -D $(TEMP_DIR)/$(KERNEL_ELF) > $(KERNEL_DASM)
