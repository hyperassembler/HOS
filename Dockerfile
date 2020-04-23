FROM fedora:latest
RUN dnf install -y cmake make clang nasm xorriso lld grub2-pc-modules grub2-tools-extra
CMD ["/bin/sh"]
