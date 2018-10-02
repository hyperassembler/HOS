#pragma once

#include "hdef.h"
#include "intr.h"
#include "kernel.h"
#include "clib.h"
#include "hal.h"

/**
 * Interrupt context structure
 */
struct interrupt_context
{
    const uint64 rip;
    const uint64 cs;
    const uint64 rflags;
    const uint64 rsp;
    const uint64 ss;
};

/**
 * IDT Defns
 */
#define GATE_DPL_0 (0ull << 13)
#define GATE_DPL_1 (1ull << 13)
#define GATE_DPL_2 (2ull << 13)
#define GATE_DPL_3 (3ull << 13)
#define GATE_PRESENT (1ull << 15)
#define GATE_TYPE_CALL (12ull << 8)
#define GATE_TYPE_INTERRUPT (14ull << 8)
#define GATE_TYPE_TRAP (15ull << 8)

#define IDT_ENTRY_NUM 256
#define IDT_ENTRY_SIZE 16

/**
 * C declaration
 */
int32
hal_interrupt_init(void);

void
hal_write_gate(void * gate, uint64  offset, uint32  selector, uint32  attr);

void
hal_set_interrupt_handler(uint64 index, void (*handler)(void));

uint32
impl_hal_set_irql(uint32 irql);

uint32
impl_hal_get_irql(void);

void
impl_hal_issue_intr(uint32 target_core, uint32 vector);

void
impl_hal_set_intr_dispatcher(k_intr_dispatcher handler);

void
impl_hal_set_exc_dispatcher(k_exc_dispatcher handler);

uint32
impl_hal_get_core_id(void);

/**
 * Exported Dispatchers for asm code
 */
void HABI
hal_interrupt_dispatcher(uint64 int_vec, struct interrupt_context *context);

void HABI
hal_exception_dispatcher(uint64 exc_vec, struct interrupt_context *context, uint32 errorcode);

/**
 * ASM declaration
 */

void HABI hal_enable_interrupt(void);

void HABI hal_disable_interrupt(void);

/**
 * System exception Handlers
 */
void HABI
hal_interrupt_handler_0(void);

void HABI
hal_interrupt_handler_1(void);

void HABI
hal_interrupt_handler_2(void);

void HABI
hal_interrupt_handler_3(void);

void HABI
hal_interrupt_handler_4(void);

void HABI
hal_interrupt_handler_5(void);

void HABI
hal_interrupt_handler_6(void);

void HABI
hal_interrupt_handler_7(void);

void HABI
hal_interrupt_handler_8(void);

void HABI

hal_interrupt_handler_9(void);

void HABI

hal_interrupt_handler_10(void);

void HABI

hal_interrupt_handler_11(void);

void HABI

hal_interrupt_handler_12(void);

void HABI

hal_interrupt_handler_13(void);

void HABI

hal_interrupt_handler_14(void);

void HABI

hal_interrupt_handler_15(void);

void HABI

hal_interrupt_handler_16(void);

void HABI

hal_interrupt_handler_17(void);

void HABI

hal_interrupt_handler_18(void);

void HABI

hal_interrupt_handler_19(void);

void HABI

hal_interrupt_handler_20(void);

void HABI

hal_interrupt_handler_21(void);

void HABI

hal_interrupt_handler_22(void);

void HABI

hal_interrupt_handler_23(void);

void HABI

hal_interrupt_handler_24(void);

void HABI

hal_interrupt_handler_25(void);

void HABI

hal_interrupt_handler_26(void);

void HABI

hal_interrupt_handler_27(void);

void HABI

hal_interrupt_handler_28(void);

void HABI

hal_interrupt_handler_29(void);

void HABI

hal_interrupt_handler_30(void);

void HABI

hal_interrupt_handler_31(void);

/**
 * Kernel defined interrupt handlers
 */
void HABI

hal_interrupt_handler_32(void);

void HABI

hal_interrupt_handler_33(void);

void HABI

hal_interrupt_handler_34(void);

void HABI

hal_interrupt_handler_35(void);

void HABI

hal_interrupt_handler_36(void);

void HABI

hal_interrupt_handler_37(void);

void HABI

hal_interrupt_handler_38(void);

void HABI

hal_interrupt_handler_39(void);

void HABI

hal_interrupt_handler_40(void);

void HABI

hal_interrupt_handler_41(void);

void HABI

hal_interrupt_handler_42(void);

void HABI

hal_interrupt_handler_43(void);

void HABI

hal_interrupt_handler_44(void);

void HABI

hal_interrupt_handler_45(void);

void HABI

hal_interrupt_handler_46(void);

void HABI

hal_interrupt_handler_47(void);

void HABI

hal_interrupt_handler_48(void);

void HABI

hal_interrupt_handler_49(void);

void HABI

hal_interrupt_handler_50(void);

void HABI

hal_interrupt_handler_51(void);

void HABI

hal_interrupt_handler_52(void);

void HABI

hal_interrupt_handler_53(void);

void HABI

hal_interrupt_handler_54(void);

void HABI

hal_interrupt_handler_55(void);

void HABI

hal_interrupt_handler_56(void);

void HABI

hal_interrupt_handler_57(void);

void HABI

hal_interrupt_handler_58(void);

void HABI

hal_interrupt_handler_59(void);

void HABI

hal_interrupt_handler_60(void);

void HABI

hal_interrupt_handler_61(void);

void HABI

hal_interrupt_handler_62(void);

void HABI

hal_interrupt_handler_63(void);

void HABI

hal_interrupt_handler_64(void);

void HABI

hal_interrupt_handler_65(void);

void HABI

hal_interrupt_handler_66(void);

void HABI

hal_interrupt_handler_67(void);

void HABI

hal_interrupt_handler_68(void);

void HABI

hal_interrupt_handler_69(void);

void HABI

hal_interrupt_handler_70(void);

void HABI

hal_interrupt_handler_71(void);

void HABI

hal_interrupt_handler_72(void);

void HABI

hal_interrupt_handler_73(void);

void HABI

hal_interrupt_handler_74(void);

void HABI

hal_interrupt_handler_75(void);

void HABI

hal_interrupt_handler_76(void);

void HABI

hal_interrupt_handler_77(void);

void HABI

hal_interrupt_handler_78(void);

void HABI

hal_interrupt_handler_79(void);

void HABI

hal_interrupt_handler_80(void);

void HABI

hal_interrupt_handler_81(void);

void HABI

hal_interrupt_handler_82(void);

void HABI

hal_interrupt_handler_83(void);

void HABI

hal_interrupt_handler_84(void);

void HABI

hal_interrupt_handler_85(void);

void HABI

hal_interrupt_handler_86(void);

void HABI

hal_interrupt_handler_87(void);

void HABI

hal_interrupt_handler_88(void);

void HABI

hal_interrupt_handler_89(void);

void HABI

hal_interrupt_handler_90(void);

void HABI

hal_interrupt_handler_91(void);

void HABI

hal_interrupt_handler_92(void);

void HABI

hal_interrupt_handler_93(void);

void HABI

hal_interrupt_handler_94(void);

void HABI

hal_interrupt_handler_95(void);

void HABI

hal_interrupt_handler_96(void);

void HABI

hal_interrupt_handler_97(void);

void HABI

hal_interrupt_handler_98(void);

void HABI

hal_interrupt_handler_99(void);

void HABI

hal_interrupt_handler_100(void);

void HABI

hal_interrupt_handler_101(void);

void HABI

hal_interrupt_handler_102(void);

void HABI

hal_interrupt_handler_103(void);

void HABI

hal_interrupt_handler_104(void);

void HABI

hal_interrupt_handler_105(void);

void HABI

hal_interrupt_handler_106(void);

void HABI

hal_interrupt_handler_107(void);

void HABI

hal_interrupt_handler_108(void);

void HABI

hal_interrupt_handler_109(void);

void HABI

hal_interrupt_handler_110(void);

void HABI

hal_interrupt_handler_111(void);

void HABI

hal_interrupt_handler_112(void);

void HABI

hal_interrupt_handler_113(void);

void HABI

hal_interrupt_handler_114(void);

void HABI

hal_interrupt_handler_115(void);

void HABI

hal_interrupt_handler_116(void);

void HABI

hal_interrupt_handler_117(void);

void HABI

hal_interrupt_handler_118(void);

void HABI

hal_interrupt_handler_119(void);

void HABI

hal_interrupt_handler_120(void);

void HABI

hal_interrupt_handler_121(void);

void HABI

hal_interrupt_handler_122(void);

void HABI

hal_interrupt_handler_123(void);

void HABI

hal_interrupt_handler_124(void);

void HABI

hal_interrupt_handler_125(void);

void HABI

hal_interrupt_handler_126(void);

void HABI

hal_interrupt_handler_127(void);

void HABI

hal_interrupt_handler_128(void);

void HABI

hal_interrupt_handler_129(void);

void HABI

hal_interrupt_handler_130(void);

void HABI

hal_interrupt_handler_131(void);

void HABI

hal_interrupt_handler_132(void);

void HABI

hal_interrupt_handler_133(void);

void HABI

hal_interrupt_handler_134(void);

void HABI

hal_interrupt_handler_135(void);

void HABI

hal_interrupt_handler_136(void);

void HABI

hal_interrupt_handler_137(void);

void HABI

hal_interrupt_handler_138(void);

void HABI

hal_interrupt_handler_139(void);

void HABI

hal_interrupt_handler_140(void);

void HABI

hal_interrupt_handler_141(void);

void HABI

hal_interrupt_handler_142(void);

void HABI

hal_interrupt_handler_143(void);

void HABI

hal_interrupt_handler_144(void);

void HABI

hal_interrupt_handler_145(void);

void HABI

hal_interrupt_handler_146(void);

void HABI

hal_interrupt_handler_147(void);

void HABI

hal_interrupt_handler_148(void);

void HABI

hal_interrupt_handler_149(void);

void HABI

hal_interrupt_handler_150(void);

void HABI

hal_interrupt_handler_151(void);

void HABI

hal_interrupt_handler_152(void);

void HABI

hal_interrupt_handler_153(void);

void HABI

hal_interrupt_handler_154(void);

void HABI

hal_interrupt_handler_155(void);

void HABI

hal_interrupt_handler_156(void);

void HABI

hal_interrupt_handler_157(void);

void HABI

hal_interrupt_handler_158(void);

void HABI

hal_interrupt_handler_159(void);

void HABI

hal_interrupt_handler_160(void);

void HABI

hal_interrupt_handler_161(void);

void HABI

hal_interrupt_handler_162(void);

void HABI

hal_interrupt_handler_163(void);

void HABI

hal_interrupt_handler_164(void);

void HABI

hal_interrupt_handler_165(void);

void HABI

hal_interrupt_handler_166(void);

void HABI

hal_interrupt_handler_167(void);

void HABI

hal_interrupt_handler_168(void);

void HABI

hal_interrupt_handler_169(void);

void HABI

hal_interrupt_handler_170(void);

void HABI

hal_interrupt_handler_171(void);

void HABI

hal_interrupt_handler_172(void);

void HABI

hal_interrupt_handler_173(void);

void HABI

hal_interrupt_handler_174(void);

void HABI

hal_interrupt_handler_175(void);

void HABI

hal_interrupt_handler_176(void);

void HABI

hal_interrupt_handler_177(void);

void HABI

hal_interrupt_handler_178(void);

void HABI

hal_interrupt_handler_179(void);

void HABI

hal_interrupt_handler_180(void);

void HABI

hal_interrupt_handler_181(void);

void HABI

hal_interrupt_handler_182(void);

void HABI

hal_interrupt_handler_183(void);

void HABI

hal_interrupt_handler_184(void);

void HABI

hal_interrupt_handler_185(void);

void HABI

hal_interrupt_handler_186(void);

void HABI

hal_interrupt_handler_187(void);

void HABI

hal_interrupt_handler_188(void);

void HABI

hal_interrupt_handler_189(void);

void HABI

hal_interrupt_handler_190(void);

void HABI

hal_interrupt_handler_191(void);

void HABI

hal_interrupt_handler_192(void);

void HABI

hal_interrupt_handler_193(void);

void HABI

hal_interrupt_handler_194(void);

void HABI

hal_interrupt_handler_195(void);

void HABI

hal_interrupt_handler_196(void);

void HABI

hal_interrupt_handler_197(void);

void HABI

hal_interrupt_handler_198(void);

void HABI

hal_interrupt_handler_199(void);

void HABI

hal_interrupt_handler_200(void);

void HABI

hal_interrupt_handler_201(void);

void HABI

hal_interrupt_handler_202(void);

void HABI

hal_interrupt_handler_203(void);

void HABI

hal_interrupt_handler_204(void);

void HABI

hal_interrupt_handler_205(void);

void HABI

hal_interrupt_handler_206(void);

void HABI

hal_interrupt_handler_207(void);

void HABI

hal_interrupt_handler_208(void);

void HABI

hal_interrupt_handler_209(void);

void HABI

hal_interrupt_handler_210(void);

void HABI

hal_interrupt_handler_211(void);

void HABI

hal_interrupt_handler_212(void);

void HABI

hal_interrupt_handler_213(void);

void HABI

hal_interrupt_handler_214(void);

void HABI

hal_interrupt_handler_215(void);

void HABI

hal_interrupt_handler_216(void);

void HABI

hal_interrupt_handler_217(void);

void HABI

hal_interrupt_handler_218(void);

void HABI

hal_interrupt_handler_219(void);

void HABI

hal_interrupt_handler_220(void);

void HABI

hal_interrupt_handler_221(void);

void HABI

hal_interrupt_handler_222(void);

void HABI

hal_interrupt_handler_223(void);

void HABI

hal_interrupt_handler_224(void);

void HABI

hal_interrupt_handler_225(void);

void HABI

hal_interrupt_handler_226(void);

void HABI

hal_interrupt_handler_227(void);

void HABI

hal_interrupt_handler_228(void);

void HABI

hal_interrupt_handler_229(void);

void HABI

hal_interrupt_handler_230(void);

void HABI

hal_interrupt_handler_231(void);

void HABI

hal_interrupt_handler_232(void);

void HABI

hal_interrupt_handler_233(void);

void HABI

hal_interrupt_handler_234(void);

void HABI

hal_interrupt_handler_235(void);

void HABI

hal_interrupt_handler_236(void);

void HABI

hal_interrupt_handler_237(void);

void HABI

hal_interrupt_handler_238(void);

void HABI

hal_interrupt_handler_239(void);

void HABI

hal_interrupt_handler_240(void);

void HABI

hal_interrupt_handler_241(void);

void HABI

hal_interrupt_handler_242(void);

void HABI

hal_interrupt_handler_243(void);

void HABI

hal_interrupt_handler_244(void);

void HABI

hal_interrupt_handler_245(void);

void HABI

hal_interrupt_handler_246(void);

void HABI

hal_interrupt_handler_247(void);

void HABI

hal_interrupt_handler_248(void);

void HABI

hal_interrupt_handler_249(void);

void HABI

hal_interrupt_handler_250(void);

void HABI

hal_interrupt_handler_251(void);

void HABI

hal_interrupt_handler_252(void);

void HABI

hal_interrupt_handler_253(void);

void HABI

hal_interrupt_handler_254(void);

void HABI

hal_interrupt_handler_255(void);
