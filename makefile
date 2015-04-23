ASM = nasm

CC = gcc

LD = ld
#Recursive Wildcard
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

#x86 vars

C_SRC_PATH_32 := x86/src/c

ASM_SRC_PATH_32 := x86/src/asm

C_FLAGS_32 := -m32 -std=c11 -c -fno-stack-protector -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -masm=intel -Wall -Wextra

ASM_FLAGS_32 := -f elf32 -I $(ASM_SRC_PATH_32)/

LD_FLAGS_32 := -melf_i386

LD_SCRIPT_32 := build/link32.ld

#x64 vars

C_SRC_PATH_64 := x64/src/c

ASM_SRC_PATH_64 := x64/src/asm

C_FLAGS_64 := -m64 -std=c11 -c -fno-stack-protector -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -masm=intel -Wall -Wextra

ASM_FLAGS_64 := -f elf64 -I $(ASM_SRC_PATH_64)/

LD_FLAGS_64 := -melf_x86_64

LD_SCRIPT_64 := build/link64.ld



GRUB_CFG := build/grub.cfg

OUTPUT_DIR := temp



#C source code

C_FILES_32 := $(call rwildcard, $(C_SRC_PATH_32), *.c)

C_FILES_64 := $(call rwildcard, $(C_SRC_PATH_64), *.c)



#ASM source code

ASM_FILES_32 := $(call rwildcard, $(ASM_SRC_PATH_32), *.asm)

ASM_FILES_64 := $(call rwildcard, $(ASM_SRC_PATH_64), *.asm)


#bin files

KERNEL_BIN_32 := kernel32.bin

KERNEL_BIN_64 := kernel64.bin


#Object files

C_OBJ_FILES_32 := $(C_FILES_32:.c=.o32)

C_OBJ_FILES_64 := $(C_FILES_64:.c=.o64)

ASM_OBJ_FILES_32 := $(ASM_FILES_32:.asm=.oa32)

ASM_OBJ_FILES_64 := $(ASM_FILES_64:.asm=.oa64)

ALL_OBJ_FILES_32 := $(addprefix $(OUTPUT_DIR)/,$(subst /,_,$(C_OBJ_FILES_32))) $(addprefix $(OUTPUT_DIR)/,$(subst /,_,$(ASM_OBJ_FILES_32)))

ALL_OBJ_FILES_64 := $(addprefix $(OUTPUT_DIR)/,$(subst /,_,$(C_OBJ_FILES_64))) $(addprefix $(OUTPUT_DIR)/,$(subst /,_,$(ASM_OBJ_FILES_64)))



print_source:

	$(info ${C_OBJ_FILES_32})



all : init compile link buildiso clean



init:

	sudo mkdir $(OUTPUT_DIR)



compile: $(C_OBJ_FILES_32) $(ASM_OBJ_FILES_32) $(C_OBJ_FILES_64) $(ASM_OBJ_FILES_64)



link: $(KERNEL_BIN_32) $(KERNEL_BIN_64)



clean:

	sudo rm -rf $(OUTPUT_DIR)

buildiso:

	sudo mkdir $(OUTPUT_DIR)/temp_iso

	sudo mkdir $(OUTPUT_DIR)/temp_iso/HOS

	sudo mkdir $(OUTPUT_DIR)/temp_iso/boot

	sudo mkdir $(OUTPUT_DIR)/temp_iso/boot/grub

	sudo mv $(OUTPUT_DIR)/$(KERNEL_BIN_64) $(OUTPUT_DIR)/temp_iso/HOS/kernel64

	sudo mv $(OUTPUT_DIR)/$(KERNEL_BIN_32) $(OUTPUT_DIR)/temp_iso/HOS/kernel32

	sudo cp $(GRUB_CFG) $(OUTPUT_DIR)/temp_iso/boot/grub/

	sudo grub-mkrescue -o HOS.iso $(OUTPUT_DIR)/temp_iso



%.o32: %.c

	sudo $(CC) $(C_FLAGS_32) -o $(OUTPUT_DIR)/$(subst /,_,$@) $^



%.o64: %.c

	sudo $(CC) $(C_FLAGS_64) -o $(OUTPUT_DIR)/$(subst /,_,$@) $^



%.oa32: %.asm

	sudo $(ASM) $(ASM_FLAGS_32) -o $(OUTPUT_DIR)/$(subst /,_,$@) $^



%.oa64: %.asm

	sudo $(ASM) $(ASM_FLAGS_64) -o $(OUTPUT_DIR)/$(subst /,_,$@) $^



$(KERNEL_BIN_32): $(ALL_OBJ_FILES_32)

	sudo $(LD) $(LD_FLAGS_32) -T $(LD_SCRIPT_32) -o $(OUTPUT_DIR)/$(KERNEL_BIN_32) $(ALL_OBJ_FILES_32)



$(KERNEL_BIN_64): $(ALL_OBJ_FILES_64)

	sudo $(LD) $(LD_FLAGS_64) -T $(LD_SCRIPT_64) -o $(OUTPUT_DIR)/$(KERNEL_BIN_64) $(ALL_OBJ_FILES_64)

