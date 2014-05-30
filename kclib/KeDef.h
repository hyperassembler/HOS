#ifndef _KeDef_h_
#define _KeDef_h_
// calling conventions
#define KeNativeAPI _cdecl
//asdadadadsasdadas
//Struct Naming:
//AAXXXXBB
//AA: Identifier
//BB: 32 or 64 bits
//XXXX: Name

typedef unsigned int ULONG32,*PULONG32;
typedef unsigned int DWORD,*PDWORD;
typedef unsigned long long QWORD,*PQWORD;
typedef unsigned long long ULONG64,*PULONG64;
typedef unsigned char ULONG8,*PULONG8;
typedef unsigned char BYTE,*PBYTE;
typedef unsigned short ULONG16,*PULONG16;
typedef void VOID,*PVOID;
typedef char CHAR,*PCHAR;

#define GDT_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))
#define GDT_SELECTOR_DUMMY GDT_SELECTOR(0,0)
#define GDT_SELECTOR_GRAPH GDT_SELECTOR(1,0)
#define GDT_SELECTOR_CODE GDT_SELECTOR(2,0)
#define GDT_SELECTOR_DATA GDT_SELECTOR(3,0)
#define GDT_SELECTOR_USER_DATA GDT_SELECTOR(4,3)
#define GDT_SELECTOR_USER_CODE GDT_SELECTOR(5,3)
#define GDT_SELECTOR_LDT GDT_SELECTOR(6,0)
#define GDT_SELECTOR_TSS GDT_SELECTOR(7,0)

#define GDT_INDEX_DUMMY 0
#define GDT_INDEX_GRAPH 1
#define GDT_INDEX_CODE 2
#define GDT_INDEX_DATA 3
#define GDT_INDEX_USER_CODE 4
#define GDT_INDEX_USER_DATA 5
#define GDT_INDEX_LDT 6
#define GDT_INDEX_TSS 7

#define KeKernelPhysicalAddress 0x30000
#define KeLoaderPhysicalAddress 0x90000
#define KeBootPhysicalAddress 0x07c00
#define KePDEBase 0x100000
#define KePTEBase 0x101000


#define GDT_DESCRIPTOR_NUMBER 128
#define IDT_GATE_NUMBER 256
#define MAXIMUM_PROCESS_NUMBER 1
#define PROCESS_STACK_MAX_SIZE 1024

#endif
