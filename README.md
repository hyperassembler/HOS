# Building
### Required packages
For compiling kernel only (make compile):

nasm, clang, lld, llvm

To make bootable ISO (make all):

xorriso; grub-pc-bin for bios; grub-efi-amd64-bin, mtools for UEFI.

### Compiling
Run "make all" or "make compile" in the root directory.

This will generate secxkrnl.elf, secxkrnl.dmp, (and secxkrnl.iso) in "out" folder

Run "make clean" to clean a build.


# Running
Load the iso with your favorite simulator or use "-kernel" option with QEMU.

For UEFI simulation, use qemu_bios.bin in the root dir with QEMU.

# C++
I would like my kernel code to be explicit so that I can reason about performance, memory allocation/deallocation. That mostly means when I look at a statement I know exactly what it does. 
The philosophy overlaps with Go's design quite a lot: https://commandcenter.blogspot.com/2012/06/less-is-exponentially-more.html. 

Using fully-featured C++ is overly complicated for kernels and I'm dubious of OOP in general. With "modern" C++ sometimes I find myself struggling more with the language itself than getting work done. Although the kernel is compiled with a C++ compiler, the base is very much C and we only add few nice things we can benefit from C++:

## Stronger types
C++ is stronger typed than C. Simply compiling the kernel itself with a C++ compiler provides more type safety than C.

## C++ style casts (no dynamic_cast)
They are compile time casts so no runtime overhead. They provide a bit better type safety than C style casts. The only two casts we would need are probably const_cast and reinterpret_cast.

## template
For type safety for data structures. Linux's list.h isn't type safe. FreeBSD's queue.h tries to mimic templates with macros, which is less elegant than just using template.

## namespace
Oh boy how I wish C standard would include namespace, if it weren't for backward compaibility and stable ABI.

## Banned features worth mentioning
This list explains SOME of the banned features that might seem useful.

### Class and ctors/dtors
All data structures should be POD structs. Basically it means a struct without user-defined constructors and destructors. The reason is 1. encapsulation is pretty useless in the kernel. 2. constructors and destructors are implicitly called when declaring objects and when objects go out of scope, which violates explicitness. 3. Everything becomes easy to reason about. Now I don't need to worry about "Object obj;" implicitly invoking the  allocator, acquiring a lock and querying a SQL database.

RAII which relies on ctors/dtors sure is nice but IMO Golang's defer is a much better approach being expressive as well as functionally "similar". C has the extension "cleanup" supported by GCC and Clang which does the same thing as defer. I'll investigate the latter.

### Member methods
Member methods should just be functions that take the struct as a parameter. It's what C++ does internally anyway. Adding member methods also obfuscates the PODness of structs, ergo banned.

### Inheritance
Inheritance is banned except for describing interfaces like file descriptors and other possible places that might benefit from it. Even then it shouldn't be abused - e.g. the kernel does NOT need a generic list interface with a billion implementations.

### Function overloading
Just give it an expressive enough name. Function overloading is obfuscation.

### Operator overriding
Think about what "f();" could mean in C++ and the code executed by "a + b;". Need I say more?

### References
I don't like mixing references with pointers. I don't find reference offering much more than raw pointers. 

### RTTI and Exceptions
Totally useless for kernels.