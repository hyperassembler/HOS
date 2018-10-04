#!/bin/bash
# Bootstrap debian 9 + clang-6.0 backport environment
# Personal use only
export CC_ENV=clang-6.0
export LD_ENV=lld-6.0
export DAS_ENV=llvm-objdump-6.0
export AS_ENV=nasm