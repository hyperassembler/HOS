#ifndef HAL_INTR_H
#define HAL_INTR_H

#include "type.h"
#include "kernel/hal/intr.h"

/**
 * Interrupt context structure
 */
typedef struct
{
	const uint64 rip;
	const uint64 cs;
	const uint64 rflags;
	const uint64 rsp;
	const uint64 ss;
} hal_interrupt_context_t;


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
 * intr.h
 */

int32 SXAPI hal_interrupt_init(void);

void SXAPI hal_write_gate(void *const gate,
                          uint64 const offset,
                          uint32 const selector,
                          uint32 const attr);

void SXAPI hal_set_interrupt_handler(uint64 index, void (*handler)(void));


/**
 * Dispatchers for asm code
 */
void SXAPI hal_interrupt_dispatcher(uint64 int_vec, hal_interrupt_context_t *context);

void SXAPI hal_exception_dispatcher(uint64 exc_vec, hal_interrupt_context_t *context, uint64 errorcode);


/**
 * System exception Handlers
 */
extern SXAPI void hal_interrupt_handler_0(void);

extern SXAPI void hal_interrupt_handler_1(void);

extern SXAPI void hal_interrupt_handler_2(void);

extern SXAPI void hal_interrupt_handler_3(void);

extern SXAPI void hal_interrupt_handler_4(void);

extern void SXAPI hal_interrupt_handler_5(void);

extern void SXAPI hal_interrupt_handler_6(void);

extern void SXAPI hal_interrupt_handler_7(void);

extern void SXAPI hal_interrupt_handler_8(void);

extern void SXAPI hal_interrupt_handler_9(void);

extern void SXAPI hal_interrupt_handler_10(void);

extern void SXAPI hal_interrupt_handler_11(void);

extern void SXAPI hal_interrupt_handler_12(void);

extern void SXAPI hal_interrupt_handler_13(void);

extern void SXAPI hal_interrupt_handler_14(void);

extern void SXAPI hal_interrupt_handler_15(void);

extern void SXAPI hal_interrupt_handler_16(void);

extern void SXAPI hal_interrupt_handler_17(void);

extern void SXAPI hal_interrupt_handler_18(void);

extern void SXAPI hal_interrupt_handler_19(void);

extern void SXAPI hal_interrupt_handler_20(void);

extern void SXAPI hal_interrupt_handler_21(void);

extern void SXAPI hal_interrupt_handler_22(void);

extern void SXAPI hal_interrupt_handler_23(void);

extern void SXAPI hal_interrupt_handler_24(void);

extern void SXAPI hal_interrupt_handler_25(void);

extern void SXAPI hal_interrupt_handler_26(void);

extern void SXAPI hal_interrupt_handler_27(void);

extern void SXAPI hal_interrupt_handler_28(void);

extern void SXAPI hal_interrupt_handler_29(void);

extern void SXAPI hal_interrupt_handler_30(void);

extern void SXAPI hal_interrupt_handler_31(void);

/**
 * Kernel defined interrupt handlers
 */
extern void SXAPI hal_interrupt_handler_32(void);

extern void SXAPI hal_interrupt_handler_33(void);

extern void SXAPI hal_interrupt_handler_34(void);

extern void SXAPI hal_interrupt_handler_35(void);

extern void SXAPI hal_interrupt_handler_36(void);

extern void SXAPI hal_interrupt_handler_37(void);

extern void SXAPI hal_interrupt_handler_38(void);

extern void SXAPI hal_interrupt_handler_39(void);

extern void SXAPI hal_interrupt_handler_40(void);

extern void SXAPI hal_interrupt_handler_41(void);

extern void SXAPI hal_interrupt_handler_42(void);

extern void SXAPI hal_interrupt_handler_43(void);

extern void SXAPI hal_interrupt_handler_44(void);

extern void SXAPI hal_interrupt_handler_45(void);

extern void SXAPI hal_interrupt_handler_46(void);

extern void SXAPI hal_interrupt_handler_47(void);

extern void SXAPI hal_interrupt_handler_48(void);

extern void SXAPI hal_interrupt_handler_49(void);

extern void SXAPI hal_interrupt_handler_50(void);

extern void SXAPI hal_interrupt_handler_51(void);

extern void SXAPI hal_interrupt_handler_52(void);

extern void SXAPI hal_interrupt_handler_53(void);

extern void SXAPI hal_interrupt_handler_54(void);

extern void SXAPI hal_interrupt_handler_55(void);

extern void SXAPI hal_interrupt_handler_56(void);

extern void SXAPI hal_interrupt_handler_57(void);

extern void SXAPI hal_interrupt_handler_58(void);

extern void SXAPI hal_interrupt_handler_59(void);

extern void SXAPI hal_interrupt_handler_60(void);

extern void SXAPI hal_interrupt_handler_61(void);

extern void SXAPI hal_interrupt_handler_62(void);

extern void SXAPI hal_interrupt_handler_63(void);

extern void SXAPI hal_interrupt_handler_64(void);

extern void SXAPI hal_interrupt_handler_65(void);

extern void SXAPI hal_interrupt_handler_66(void);

extern void SXAPI hal_interrupt_handler_67(void);

extern void SXAPI hal_interrupt_handler_68(void);

extern void SXAPI hal_interrupt_handler_69(void);

extern void SXAPI hal_interrupt_handler_70(void);

extern void SXAPI hal_interrupt_handler_71(void);

extern void SXAPI hal_interrupt_handler_72(void);

extern void SXAPI hal_interrupt_handler_73(void);

extern void SXAPI hal_interrupt_handler_74(void);

extern void SXAPI hal_interrupt_handler_75(void);

extern void SXAPI hal_interrupt_handler_76(void);

extern void SXAPI hal_interrupt_handler_77(void);

extern void SXAPI hal_interrupt_handler_78(void);

extern void SXAPI hal_interrupt_handler_79(void);

extern void SXAPI hal_interrupt_handler_80(void);

extern void SXAPI hal_interrupt_handler_81(void);

extern void SXAPI hal_interrupt_handler_82(void);

extern void SXAPI hal_interrupt_handler_83(void);

extern void SXAPI hal_interrupt_handler_84(void);

extern void SXAPI hal_interrupt_handler_85(void);

extern void SXAPI hal_interrupt_handler_86(void);

extern void SXAPI hal_interrupt_handler_87(void);

extern void SXAPI hal_interrupt_handler_88(void);

extern void SXAPI hal_interrupt_handler_89(void);

extern void SXAPI hal_interrupt_handler_90(void);

extern void SXAPI hal_interrupt_handler_91(void);

extern void SXAPI hal_interrupt_handler_92(void);

extern void SXAPI hal_interrupt_handler_93(void);

extern void SXAPI hal_interrupt_handler_94(void);

extern void SXAPI hal_interrupt_handler_95(void);

extern void SXAPI hal_interrupt_handler_96(void);

extern void SXAPI hal_interrupt_handler_97(void);

extern void SXAPI hal_interrupt_handler_98(void);

extern void SXAPI hal_interrupt_handler_99(void);

extern void SXAPI hal_interrupt_handler_100(void);

extern void SXAPI hal_interrupt_handler_101(void);

extern void SXAPI hal_interrupt_handler_102(void);

extern void SXAPI hal_interrupt_handler_103(void);

extern void SXAPI hal_interrupt_handler_104(void);

extern void SXAPI hal_interrupt_handler_105(void);

extern void SXAPI hal_interrupt_handler_106(void);

extern void SXAPI hal_interrupt_handler_107(void);

extern void SXAPI hal_interrupt_handler_108(void);

extern void SXAPI hal_interrupt_handler_109(void);

extern void SXAPI hal_interrupt_handler_110(void);

extern void SXAPI hal_interrupt_handler_111(void);

extern void SXAPI hal_interrupt_handler_112(void);

extern void SXAPI hal_interrupt_handler_113(void);

extern void SXAPI hal_interrupt_handler_114(void);

extern void SXAPI hal_interrupt_handler_115(void);

extern void SXAPI hal_interrupt_handler_116(void);

extern void SXAPI hal_interrupt_handler_117(void);

extern void SXAPI hal_interrupt_handler_118(void);

extern void SXAPI hal_interrupt_handler_119(void);

extern void SXAPI hal_interrupt_handler_120(void);

extern void SXAPI hal_interrupt_handler_121(void);

extern void SXAPI hal_interrupt_handler_122(void);

extern void SXAPI hal_interrupt_handler_123(void);

extern void SXAPI hal_interrupt_handler_124(void);

extern void SXAPI hal_interrupt_handler_125(void);

extern void SXAPI hal_interrupt_handler_126(void);

extern void SXAPI hal_interrupt_handler_127(void);

extern void SXAPI hal_interrupt_handler_128(void);

extern void SXAPI hal_interrupt_handler_129(void);

extern void SXAPI hal_interrupt_handler_130(void);

extern void SXAPI hal_interrupt_handler_131(void);

extern void SXAPI hal_interrupt_handler_132(void);

extern void SXAPI hal_interrupt_handler_133(void);

extern void SXAPI hal_interrupt_handler_134(void);

extern void SXAPI hal_interrupt_handler_135(void);

extern void SXAPI hal_interrupt_handler_136(void);

extern void SXAPI hal_interrupt_handler_137(void);

extern void SXAPI hal_interrupt_handler_138(void);

extern void SXAPI hal_interrupt_handler_139(void);

extern void SXAPI hal_interrupt_handler_140(void);

extern void SXAPI hal_interrupt_handler_141(void);

extern void SXAPI hal_interrupt_handler_142(void);

extern void SXAPI hal_interrupt_handler_143(void);

extern void SXAPI hal_interrupt_handler_144(void);

extern void SXAPI hal_interrupt_handler_145(void);

extern void SXAPI hal_interrupt_handler_146(void);

extern void SXAPI hal_interrupt_handler_147(void);

extern void SXAPI hal_interrupt_handler_148(void);

extern void SXAPI hal_interrupt_handler_149(void);

extern void SXAPI hal_interrupt_handler_150(void);

extern void SXAPI hal_interrupt_handler_151(void);

extern void SXAPI hal_interrupt_handler_152(void);

extern void SXAPI hal_interrupt_handler_153(void);

extern void SXAPI hal_interrupt_handler_154(void);

extern void SXAPI hal_interrupt_handler_155(void);

extern void SXAPI hal_interrupt_handler_156(void);

extern void SXAPI hal_interrupt_handler_157(void);

extern void SXAPI hal_interrupt_handler_158(void);

extern void SXAPI hal_interrupt_handler_159(void);

extern void SXAPI hal_interrupt_handler_160(void);

extern void SXAPI hal_interrupt_handler_161(void);

extern void SXAPI hal_interrupt_handler_162(void);

extern void SXAPI hal_interrupt_handler_163(void);

extern void SXAPI hal_interrupt_handler_164(void);

extern void SXAPI hal_interrupt_handler_165(void);

extern void SXAPI hal_interrupt_handler_166(void);

extern void SXAPI hal_interrupt_handler_167(void);

extern void SXAPI hal_interrupt_handler_168(void);

extern void SXAPI hal_interrupt_handler_169(void);

extern void SXAPI hal_interrupt_handler_170(void);

extern void SXAPI hal_interrupt_handler_171(void);

extern void SXAPI hal_interrupt_handler_172(void);

extern void SXAPI hal_interrupt_handler_173(void);

extern void SXAPI hal_interrupt_handler_174(void);

extern void SXAPI hal_interrupt_handler_175(void);

extern void SXAPI hal_interrupt_handler_176(void);

extern void SXAPI hal_interrupt_handler_177(void);

extern void SXAPI hal_interrupt_handler_178(void);

extern void SXAPI hal_interrupt_handler_179(void);

extern void SXAPI hal_interrupt_handler_180(void);

extern void SXAPI hal_interrupt_handler_181(void);

extern void SXAPI hal_interrupt_handler_182(void);

extern void SXAPI hal_interrupt_handler_183(void);

extern void SXAPI hal_interrupt_handler_184(void);

extern void SXAPI hal_interrupt_handler_185(void);

extern void SXAPI hal_interrupt_handler_186(void);

extern void SXAPI hal_interrupt_handler_187(void);

extern void SXAPI hal_interrupt_handler_188(void);

extern void SXAPI hal_interrupt_handler_189(void);

extern void SXAPI hal_interrupt_handler_190(void);

extern void SXAPI hal_interrupt_handler_191(void);

extern void SXAPI hal_interrupt_handler_192(void);

extern void SXAPI hal_interrupt_handler_193(void);

extern void SXAPI hal_interrupt_handler_194(void);

extern void SXAPI hal_interrupt_handler_195(void);

extern void SXAPI hal_interrupt_handler_196(void);

extern void SXAPI hal_interrupt_handler_197(void);

extern void SXAPI hal_interrupt_handler_198(void);

extern void SXAPI hal_interrupt_handler_199(void);

extern void SXAPI hal_interrupt_handler_200(void);

extern void SXAPI hal_interrupt_handler_201(void);

extern void SXAPI hal_interrupt_handler_202(void);

extern void SXAPI hal_interrupt_handler_203(void);

extern void SXAPI hal_interrupt_handler_204(void);

extern void SXAPI hal_interrupt_handler_205(void);

extern void SXAPI hal_interrupt_handler_206(void);

extern void SXAPI hal_interrupt_handler_207(void);

extern void SXAPI hal_interrupt_handler_208(void);

extern void SXAPI hal_interrupt_handler_209(void);

extern void SXAPI hal_interrupt_handler_210(void);

extern void SXAPI hal_interrupt_handler_211(void);

extern void SXAPI hal_interrupt_handler_212(void);

extern void SXAPI hal_interrupt_handler_213(void);

extern void SXAPI hal_interrupt_handler_214(void);

extern void SXAPI hal_interrupt_handler_215(void);

extern void SXAPI hal_interrupt_handler_216(void);

extern void SXAPI hal_interrupt_handler_217(void);

extern void SXAPI hal_interrupt_handler_218(void);

extern void SXAPI hal_interrupt_handler_219(void);

extern void SXAPI hal_interrupt_handler_220(void);

extern void SXAPI hal_interrupt_handler_221(void);

extern void SXAPI hal_interrupt_handler_222(void);

extern void SXAPI hal_interrupt_handler_223(void);

extern void SXAPI hal_interrupt_handler_224(void);

extern void SXAPI hal_interrupt_handler_225(void);

extern void SXAPI hal_interrupt_handler_226(void);

extern void SXAPI hal_interrupt_handler_227(void);

extern void SXAPI hal_interrupt_handler_228(void);

extern void SXAPI hal_interrupt_handler_229(void);

extern void SXAPI hal_interrupt_handler_230(void);

extern void SXAPI hal_interrupt_handler_231(void);

extern void SXAPI hal_interrupt_handler_232(void);

extern void SXAPI hal_interrupt_handler_233(void);

extern void SXAPI hal_interrupt_handler_234(void);

extern void SXAPI hal_interrupt_handler_235(void);

extern void SXAPI hal_interrupt_handler_236(void);

extern void SXAPI hal_interrupt_handler_237(void);

extern void SXAPI hal_interrupt_handler_238(void);

extern void SXAPI hal_interrupt_handler_239(void);

extern void SXAPI hal_interrupt_handler_240(void);

extern void SXAPI hal_interrupt_handler_241(void);

extern void SXAPI hal_interrupt_handler_242(void);

extern void SXAPI hal_interrupt_handler_243(void);

extern void SXAPI hal_interrupt_handler_244(void);

extern void SXAPI hal_interrupt_handler_245(void);

extern void SXAPI hal_interrupt_handler_246(void);

extern void SXAPI hal_interrupt_handler_247(void);

extern void SXAPI hal_interrupt_handler_248(void);

extern void SXAPI hal_interrupt_handler_249(void);

extern void SXAPI hal_interrupt_handler_250(void);

extern void SXAPI hal_interrupt_handler_251(void);

extern void SXAPI hal_interrupt_handler_252(void);

extern void SXAPI hal_interrupt_handler_253(void);

extern void SXAPI hal_interrupt_handler_254(void);

extern void SXAPI hal_interrupt_handler_255(void);


#endif
