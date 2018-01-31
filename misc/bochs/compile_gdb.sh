cd src
./configure   --enable-cpu-level=6 \
              --enable-all-optimizations \
              --enable-x86-64 \
              --enable-pci \
              --enable-vmx \
              --enable-disasm \
              --enable-logging \
              --enable-fpu \
              --enable-3dnow \
              --enable-sb16=dummy \
              --enable-cdrom \
              --enable-iodebug \
              --disable-plugins \
              --disable-docbook \
	          --enable-gdb-stub \
              --with-x --with-x11 --with-term --with-sdl

make

