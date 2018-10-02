#!/bin/bash
qemu-system-x86_64 -bios qemu_bios.bin -vnc :1235 -monitor stdio -cdrom out/secxkrnl.iso -s -S
