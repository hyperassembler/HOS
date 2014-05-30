#ifndef _KeDef_h_
#define _KeDef_h_
//Types
typedef unsigned int ULONG32,*PULONG32;
typedef unsigned int DWORD,*PDWORD;
typedef unsigned long long QWORD,*PQWORD;
typedef unsigned long long ULONG64,*PULONG64;
typedef unsigned char ULONG8,*PULONG8;
typedef unsigned char BYTE,*PBYTE;
typedef unsigned short ULONG16,*PULONG16;
typedef void VOID,*PVOID;
typedef char CHAR,*PCHAR;

//Physical Addresses
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


// #define LDT_SELECTOR_CODE 0
// #define LDT_SELECTOR_DATA 8
// #define LDT_SELECTOR_GRAPH 16
// #define LDT_INDEX_CODE 0
// #define LDT_INDEX_DATA 1
// #define LDT_INDEX_GRAPH 2

#define KeKernelPhysicalAddress 0x30000
#define KeLoaderPhysicalAddress 0x90000
#define KeBootPhysicalAddress 0x07c00
#define KePDEBase 0x100000
#define KePTEBase 0x101000

//DTs
#define GDT_DESCRIPTOR_NUMBER 128
#define IDT_GATE_NUMBER 256
#define LDT_DESCRIPTOR_NUMBER 3
#define MAXIMUM_PROCESS_NUMBER 1
#define PROCESS_STACK_MAX_SIZE 1024

//SegmentDescriptor_Attribute 

//IDT
#define INT_M_CTL 0x20
#define INT_M_CTLMASK 0x21
#define INT_S_CTL 0xA0
#define INT_S_CTLMASK 0xA1
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

#define	DA_32			0x4000	/* 32 位段				*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节			*/
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90	/* 存在的只读数据段类型值		*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值		*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值	*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值		*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值		*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值		*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82	/* 局部描述符表段类型值			*/
#define	DA_TaskGate		0x85	/* 任务门类型值				*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值		*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值			*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值			*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值			*/

/* 选择子类型值说明 */
/* 其中, SA_ : Selector Attribute */
#define	SA_RPL_MASK	0xFFFC
#define	SA_RPL0		0
#define	SA_RPL1		1
#define	SA_RPL2		2
#define	SA_RPL3		3

#define	SA_TI_MASK	0xFFFB
#define	SA_TIG		0
#define	SA_TIL		4

/* RPL */
#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3


//
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10

//System Data Structures
#pragma  pack(push)
#pragma  pack(1)
typedef struct _GDT_DESCRIPTOR
{
	ULONG16 LimitLow;
	ULONG16 BaseLow;
	ULONG8 BaseMid;
	ULONG8 Attr1;
	ULONG8 LimitHigh_Attr2;
	ULONG8 BaseHigh;
} GDT_DESCRIPTOR,*PGDT_DESCRIPTOR;

typedef GDT_DESCRIPTOR LDT_DESCRIPTOR,*PLDT_DESCRIPTRO;
typedef struct _GATE
{
	ULONG16 OffsetLow;
	ULONG16 Selector;
	ULONG8 Count;
	ULONG8 Attr;
	ULONG16 OffsetHigh;
} GATE,*PGATE;

//Write GDT
#define KeWriteGDT_AVL_1 0x00100000
#define KeWriteGDT_AVL_0 0x0
#define KeWriteGDT_DB_0 0x0
#define KeWriteGDT_DB_1 0x00400000
#define KeWriteGDT_P_0 0x0
#define KeWriteGDT_P_1 0x00008000
#define KeWriteGDT_G_0 0x0
#define KeWriteGDT_G_1 0x00800000
#define KeWriteGDT_L_0 0x0
#define KeWriteGDT_L_1 0x00200000
#define KeWriteGDT_DPL_0 0x0
#define KeWriteGDT_DPL_1 0x00002000
#define KeWriteGDT_DPL_2 0x00004000
#define KeWriteGDT_DPL_3 0x00006000
#define KeWriteGDT_S_0 0x0
#define KeWriteGDT_S_1 0x00001000
#define KeWriteGDT_Type_Data_ReadOnly 0x0
#define KeWriteGDT_Type_Data_ReadOnlyAccessed 0x00000100
#define KeWriteGDT_Type_Data_ReadWrite 0x00000200
#define KeWriteGDT_Type_Data_ReadWriteAccessed 0x00000300
#define KeWriteGDT_Type_Data_ReadOnlyExpandDown 0x00000400
#define KeWriteGDT_Type_Data_ReadOnlyExpandDownAccessed 0x00000500
#define KeWriteGDT_Type_Data_ReadWriteExpandDown 0x00000600
#define KeWriteGDT_Type_Data_ReadWriteExpandDownAccessed 0x00000700
#define KeWriteGDT_Type_Code_ExecuteOnly 0x00000800
#define KeWriteGDT_Type_Code_ExecuteOnlyAccessed 0x00000900
#define KeWriteGDT_Type_Code_ExecuteRead 0x00000A00
#define KeWriteGDT_Type_Code_ExecuteReadAccessed 0x00000B00
#define KeWriteGDT_Type_Code_ExecuteOnlyConforming 0x00000C00
#define KeWriteGDT_Type_Code_ExecuteOnlyConformingAccessed 0x00000D00
#define KeWriteGDT_Type_Code_ExecuteReadConforming 0x00000E00
#define KeWriteGDT_Type_Code_ExecuteReadConformingAccessed 0x00000F00
#define KeWriteGDT_Type_System_Reserved1 0x00000000
#define KeWriteGDT_Type_System_16BitsTssAvailable 0x00000100
#define KeWriteGDT_Type_System_LDT 0x00000200
#define KeWriteGDT_Type_System_16BitsTssBusy 0x00000300
#define KeWriteGDT_Type_System_16BitsCallGate 0x00000400
#define KeWriteGDT_Type_System_TaskGate 0x00000500
#define KeWriteGDT_Type_System_16BitsInterruptGate 0x00000600
#define KeWriteGDT_Type_System_16BitsTrapGate 0x00000700
#define KeWriteGDT_Type_System_Reserved2 0x00000800
#define KeWriteGDT_Type_System_32BitsTSSAvailable 0x00000900
#define KeWriteGDT_Type_System_Reserved3 0x00000A00
#define KeWriteGDT_Type_System_32BitsTSSBusy 0x00000B00
#define KeWriteGDT_Type_System_32BitsCallGate 0x00000C00
#define KeWriteGDT_Type_System_Reserved4 0x00000D00
#define KeWriteGDT_Type_System_32BitsInterruptGate 0x00000E00
#define KeWriteGDT_Type_System_32BitsTrapGate 0x00000F00

typedef struct _Write_GDT_DESCRIPTOR
{
	ULONG32 Base; //+0
	ULONG32 Limit; //+4
	ULONG32 AVL; //+8
	ULONG32 DB; // +12
	ULONG32 P; //+16
	ULONG32 DPL; // +20
	ULONG32 G; //+24
	ULONG32 S; //+28
	ULONG32 Type; //+32
} Write_GDT_DESCRIPTOR, *PWrite_GDT_DESCRIPTOR;


#define KeWriteIDT_P_0 0x0
#define KeWriteIDT_P_1 0x00008000
#define KeWriteIDT_DPL_0 0x0
#define KeWriteIDT_DPL_1 0x00002000
#define KeWriteIDT_DPL_2 0x00004000
#define KeWriteIDT_DPL_3 0x00006000
#define KeWriteIDT_D_0 0x0
#define KeWriteIDT_D_1 2048
#define KeWriteIDT_GateType_TaskGate 256
#define KeWriteIDT_GateType_InterruptGate 1536
#define KeWriteIDT_GateType_TrapGate 1792

//IDT Descriptor
typedef struct _Write_IDT_DESCRIPTOR
{
	ULONG32 D;
	ULONG32 Offset;
	ULONG32 Selector;
	ULONG32 DPL;
	ULONG32 P;
	ULONG32 GateType;
} Write_IDT_DESCRIPTOR, *PWrite_IDT_DESCRIPTOR;


//Process Stuff
typedef struct _hProcess_RestartStack
{
	ULONG32 gs;
	ULONG32 fs;
	ULONG32 es;
	ULONG32 ds;
	ULONG32 edi;
	ULONG32 esi;
	ULONG32 ebp;
	ULONG32 Kernel_esp;
	ULONG32 ebx;
	ULONG32 edx;
	ULONG32 ecx;
	ULONG32 eax;
	ULONG32 ReturnAddress;
	ULONG32 eip;
	ULONG32 cs;
	ULONG32 eFlags;
	ULONG32 esp;
	ULONG32 ss;
} hProcess_RestartStack,*PhProcess_RestartStack;

typedef ULONG32 PROCESSPRIVILEGE;
#define Process_Priviliege_User 0x01
#define Process_Priviliege_Kernel 0x00

typedef struct _hProcess
{
	hProcess_RestartStack RestartStack;
	//ULONG16 LDTSelector;
	//LDT_DESCRIPTOR ProcessLDT[LDT_DESCRIPTOR_NUMBER];
	ULONG32 ProcessID;
	PCHAR ProcessName;
	PROCESSPRIVILEGE Privilege;
} hPrcocess,*PhProcess;

typedef struct _TSS {
	ULONG32 BackLine;
	ULONG32	esp0;
	ULONG32	ss0;
	ULONG32	esp1;
	ULONG32	ss1;
	ULONG32	esp2;
	ULONG32	ss2;
	ULONG32	cr3;
	ULONG32	eip;
	ULONG32	flags;
	ULONG32	eax;
	ULONG32	ecx;
	ULONG32	edx;
	ULONG32	ebx;
	ULONG32	esp;
	ULONG32	ebp;
	ULONG32	esi;
	ULONG32	edi;
	ULONG32	es;
	ULONG32	cs;
	ULONG32	ss;
	ULONG32	ds;
	ULONG32	fs;
	ULONG32	gs;
	ULONG32	LDT;
	ULONG16	Trap;
	ULONG16	IOBase;
} TSS,*PTSS;

typedef VOID (*PExceptionHandleFunc)(VOID);

#pragma  pack(pop)

#endif
