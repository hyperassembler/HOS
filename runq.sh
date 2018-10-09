#!/bin/bash
qemu-system-x86_64 -bios qemu_bios.bin -vnc :10 -monitor stdio -cdrom out/secxkrnl.iso -s -S -no_reboot


