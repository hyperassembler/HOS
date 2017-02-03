ASM := nasm
CC := x86_64-linux-gnu-gcc
LD := x86_64-linux-gnu-gcc

#Recursive Wildcard
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
rdircard=$(sort $(dir $(call rwildcard,$1,*)))
rdircardex=$(sort $(dir $(call rwildcard,$1,$2)))

#x64 vars
LD_SCRIPT := build/linker.ld
GRUB_CFG := build/grub.cfg

SOURCE_DIR := src
TARGET_DIR := target
HEADER_DIRS := $(call rdircardex, $(SOURCE_DIR), *.h)
ALL_TARGET_DIRS := $(addprefix $(TARGET_DIR)/,$(call rdircard, $(SOURCE_DIR)))
ASM_HEADER_DIRS := $(call rdircardex, $(SOURCE_DIR), *.inc)

C_FLAGS := -std=c11 -g -c -mcmodel=kernel -fno-exceptions -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -masm=intel -Wall -Werror -Wextra -Wno-comment $(addprefix -I, $(HEADER_DIRS))
ASM_FLAGS := -f elf64 -F dwarf -g $(addprefix -I, $(ASM_HEADER_DIRS))
LD_FLAGS := -lgcc -nodefaultlibs -nostartfiles -nostdlib -Wl,-n -Wl,--build-id=none

C_SRC := $(call rwildcard, $(SOURCE_DIR), *.c)
ASM_SRC := $(call rwildcard, $(SOURCE_DIR), *.asm)

KERNEL_ELF := kernel.elf
KERNEL_DASM := kernel.dasm

#Object files
C_OBJ := $(C_SRC:.c=.o)
ASM_OBJ := $(ASM_SRC:.asm=.oasm)
ALL_OBJ := $(addprefix $(TARGET_DIR)/,$(C_OBJ)) $(addprefix $(TARGET_DIR)/,$(ASM_OBJ))


#Commands
all: init compile link buildiso disasm

init:
	mkdir -p $(ALL_TARGET_DIRS)

compile: $(C_OBJ) $(ASM_OBJ)

link: $(KERNEL_ELF)

buildiso:
	mkdir -p $(TARGET_DIR)/temp_iso/secX
	mkdir -p $(TARGET_DIR)/temp_iso/boot
	mkdir -p $(TARGET_DIR)/temp_iso/boot/grub
	cp $(TARGET_DIR)/$(KERNEL_ELF) $(TARGET_DIR)/temp_iso/secX/$(KERNEL_ELF)
	cp $(GRUB_CFG) $(TARGET_DIR)/temp_iso/boot/grub/
	grub-mkrescue -o $(TARGET_DIR)/secX.iso $(TARGET_DIR)/temp_iso

clean:
	rm -rf $(TARGET_DIR)

%.o: %.c
	$(CC) $(C_FLAGS) -o $(TARGET_DIR)/$@ $^

%.oasm: %.asm
	$(ASM) $(ASM_FLAGS) -o $(TARGET_DIR)/$@ $^

$(KERNEL_ELF): $(ALL_OBJ)
	$(LD) $(LD_FLAGS) -T $(LD_SCRIPT) -o $(TARGET_DIR)/$(KERNEL_ELF) $(ALL_OBJ)

disasm:
	objdump -M intel -D $(TARGET_DIR)/$(KERNEL_ELF) > $(TARGET_DIR)/$(KERNEL_DASM)
