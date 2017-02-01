/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "hal_arch.h"
#include "hal_intr.h"
#include "hal_print.h"
#include "hal_mem.h"
#include "sxtdlib.h"

static uint8_t _idts[HAL_CORE_COUNT][IDT_ENTRY_NUM*IDT_ENTRY_SIZE];
hal_idt_ptr_t _idt_ptrs[HAL_CORE_COUNT];
static k_intr_handler_t _intr_handler_table[HAL_CORE_COUNT][IDT_ENTRY_NUM];
static void* _intr_handler_context_table[HAL_CORE_COUNT][IDT_ENTRY_NUM];
static k_exc_handler_t _exc_handler_table[HAL_CORE_COUNT][IDT_ENTRY_NUM];


void KABI hal_write_gate(void *const gate,
                         uint64_t const offset,
                         uint32_t const selector,
                         uint32_t const attr)
{
    ((uint8_t *) gate)[0] = (uint8_t) (offset & 0xFF);
    ((uint8_t *) gate)[1] = (uint8_t) ((offset >> 8) & 0xFF);
    ((uint8_t *) gate)[2] = (uint8_t) (selector & 0xFF);
    ((uint8_t *) gate)[3] = (uint8_t) ((selector >> 8) & 0xFF);
    ((uint8_t *) gate)[4] = (uint8_t) (attr & 0xFF);
    ((uint8_t *) gate)[5] = (uint8_t) ((attr >> 8) & 0xFF);
    ((uint8_t *) gate)[6] = (uint8_t) ((offset >> 16) & 0xFF);
    ((uint8_t *) gate)[7] = (uint8_t) ((offset >> 24) & 0xFF);
    ((uint8_t *) gate)[8] = (uint8_t) ((offset >> 32) & 0xFF);
    ((uint8_t *) gate)[9] = (uint8_t) ((offset >> 40) & 0xFF);
    ((uint8_t *) gate)[10] = (uint8_t) ((offset >> 48) & 0xFF);
    ((uint8_t *) gate)[11] = (uint8_t) ((offset >> 56) & 0xFF);
    ((uint8_t *) gate)[12] = 0;
    ((uint8_t *) gate)[13] = 0;
    ((uint8_t *) gate)[14] = 0;
    ((uint8_t *) gate)[15] = 0;
    return;
}

void KABI hal_set_interrupt_handler(uint64_t index,
                                    void (*handler)(void))
{
    if (index < IDT_ENTRY_NUM)
    {
        hal_write_gate(_idts[hal_get_core_id()] + 16 * index, (uint64_t) handler, seg_selector(1, 0),
                       GATE_DPL_0 | GATE_PRESENT | GATE_TYPE_INTERRUPT);
    }
    return;
}

void KABI hal_issue_interrupt(uint32_t target_core, uint32_t vector)
{
    // TODO
    UNREFERENCED(target_core);
    UNREFERENCED(vector);
    return;
}

void KABI hal_register_interrupt_handler(uint32_t coreid, uint32_t index, k_intr_handler_t handler, void* context)
{
    if (index < IDT_ENTRY_NUM && coreid < HAL_CORE_COUNT)
    {
        _intr_handler_table[coreid][index] = handler;
        _intr_handler_context_table[coreid][index] = context;
    }
    return;
}

void KABI hal_deregister_interrupt_handler(uint32_t coreid, uint32_t index)
{
    if (index < IDT_ENTRY_NUM && coreid < HAL_CORE_COUNT)
    {
        _intr_handler_table[coreid][index] = NULL;
    }
    return;
}

void KABI hal_register_exception_handler(uint32_t coreid, uint32_t index, k_exc_handler_t handler)
{
    if (index < IDT_ENTRY_NUM && coreid < HAL_CORE_COUNT)
    {
        _exc_handler_table[coreid][index] = handler;
    }
    return;
}

void KABI hal_deregister_exception_handler(uint32_t coreid, uint32_t index)
{
    if (index < IDT_ENTRY_NUM && coreid < HAL_CORE_COUNT)
    {
        _exc_handler_table[coreid][index] = NULL;
    }
    return;
}

void KABI hal_assert(int64_t expression,
                     char *message)
{
    if (!expression)
    {
        hal_printf("HAL: Assertion failed. Detail: %s", message == NULL ? "NULL" : message);
        hal_halt_cpu();
    }
    return;
}

void KABI hal_interrupt_dispatcher(uint64_t int_vec, hal_intr_context_t *context)
{
    uint32_t coreid = hal_get_core_id();
    if (_intr_handler_table[int_vec] == NULL)
    {
        hal_printf("Unhandled interrupt %d at 0x%X.\n", int_vec, context->rip);
    }
    else
    {
        _intr_handler_table[coreid][int_vec](context, _intr_handler_context_table[coreid][int_vec]);
    }
    return;
}

void KABI hal_exception_dispatcher(uint64_t exc_vec, hal_intr_context_t* context, uint64_t errorcode)
{
    uint32_t coreid = hal_get_core_id();
    if (_exc_handler_table[exc_vec] == NULL)
    {
        hal_printf("Unhandled exception %d at 0x%X.\n", exc_vec, context->rip);
    }
    else
    {
        _exc_handler_table[coreid][exc_vec](context->rip, context->rsp, errorcode);
    }
    return;
}

static void KABI _hal_populate_idt()
{
    hal_set_interrupt_handler(0, hal_interrupt_handler_0);
    hal_set_interrupt_handler(1, hal_interrupt_handler_1);
    hal_set_interrupt_handler(2, hal_interrupt_handler_2);
    hal_set_interrupt_handler(3, hal_interrupt_handler_3);
    hal_set_interrupt_handler(4, hal_interrupt_handler_4);
    hal_set_interrupt_handler(5, hal_interrupt_handler_5);
    hal_set_interrupt_handler(6, hal_interrupt_handler_6);
    hal_set_interrupt_handler(7, hal_interrupt_handler_7);
    hal_set_interrupt_handler(8, hal_interrupt_handler_8);
    hal_set_interrupt_handler(9, hal_interrupt_handler_9);
    hal_set_interrupt_handler(10, hal_interrupt_handler_10);
    hal_set_interrupt_handler(11, hal_interrupt_handler_11);
    hal_set_interrupt_handler(12, hal_interrupt_handler_12);
    hal_set_interrupt_handler(13, hal_interrupt_handler_13);
    hal_set_interrupt_handler(14, hal_interrupt_handler_14);
    hal_set_interrupt_handler(15, hal_interrupt_handler_15);
    hal_set_interrupt_handler(16, hal_interrupt_handler_16);
    hal_set_interrupt_handler(17, hal_interrupt_handler_17);
    hal_set_interrupt_handler(18, hal_interrupt_handler_18);
    hal_set_interrupt_handler(19, hal_interrupt_handler_19);
    hal_set_interrupt_handler(20, hal_interrupt_handler_20);
    hal_set_interrupt_handler(21, hal_interrupt_handler_21);
    hal_set_interrupt_handler(22, hal_interrupt_handler_22);
    hal_set_interrupt_handler(23, hal_interrupt_handler_23);
    hal_set_interrupt_handler(24, hal_interrupt_handler_24);
    hal_set_interrupt_handler(25, hal_interrupt_handler_25);
    hal_set_interrupt_handler(26, hal_interrupt_handler_26);
    hal_set_interrupt_handler(27, hal_interrupt_handler_27);
    hal_set_interrupt_handler(28, hal_interrupt_handler_28);
    hal_set_interrupt_handler(29, hal_interrupt_handler_29);
    hal_set_interrupt_handler(30, hal_interrupt_handler_30);
    hal_set_interrupt_handler(31, hal_interrupt_handler_31);
    hal_set_interrupt_handler(32, hal_interrupt_handler_32);
    hal_set_interrupt_handler(33, hal_interrupt_handler_33);
    hal_set_interrupt_handler(34, hal_interrupt_handler_34);
    hal_set_interrupt_handler(35, hal_interrupt_handler_35);
    hal_set_interrupt_handler(36, hal_interrupt_handler_36);
    hal_set_interrupt_handler(37, hal_interrupt_handler_37);
    hal_set_interrupt_handler(38, hal_interrupt_handler_38);
    hal_set_interrupt_handler(39, hal_interrupt_handler_39);
    hal_set_interrupt_handler(40, hal_interrupt_handler_40);
    hal_set_interrupt_handler(41, hal_interrupt_handler_41);
    hal_set_interrupt_handler(42, hal_interrupt_handler_42);
    hal_set_interrupt_handler(43, hal_interrupt_handler_43);
    hal_set_interrupt_handler(44, hal_interrupt_handler_44);
    hal_set_interrupt_handler(45, hal_interrupt_handler_45);
    hal_set_interrupt_handler(46, hal_interrupt_handler_46);
    hal_set_interrupt_handler(47, hal_interrupt_handler_47);
    hal_set_interrupt_handler(48, hal_interrupt_handler_48);
    hal_set_interrupt_handler(49, hal_interrupt_handler_49);
    hal_set_interrupt_handler(50, hal_interrupt_handler_50);
    hal_set_interrupt_handler(51, hal_interrupt_handler_51);
    hal_set_interrupt_handler(52, hal_interrupt_handler_52);
    hal_set_interrupt_handler(53, hal_interrupt_handler_53);
    hal_set_interrupt_handler(54, hal_interrupt_handler_54);
    hal_set_interrupt_handler(55, hal_interrupt_handler_55);
    hal_set_interrupt_handler(56, hal_interrupt_handler_56);
    hal_set_interrupt_handler(57, hal_interrupt_handler_57);
    hal_set_interrupt_handler(58, hal_interrupt_handler_58);
    hal_set_interrupt_handler(59, hal_interrupt_handler_59);
    hal_set_interrupt_handler(60, hal_interrupt_handler_60);
    hal_set_interrupt_handler(61, hal_interrupt_handler_61);
    hal_set_interrupt_handler(62, hal_interrupt_handler_62);
    hal_set_interrupt_handler(63, hal_interrupt_handler_63);
    hal_set_interrupt_handler(64, hal_interrupt_handler_64);
    hal_set_interrupt_handler(65, hal_interrupt_handler_65);
    hal_set_interrupt_handler(66, hal_interrupt_handler_66);
    hal_set_interrupt_handler(67, hal_interrupt_handler_67);
    hal_set_interrupt_handler(68, hal_interrupt_handler_68);
    hal_set_interrupt_handler(69, hal_interrupt_handler_69);
    hal_set_interrupt_handler(70, hal_interrupt_handler_70);
    hal_set_interrupt_handler(71, hal_interrupt_handler_71);
    hal_set_interrupt_handler(72, hal_interrupt_handler_72);
    hal_set_interrupt_handler(73, hal_interrupt_handler_73);
    hal_set_interrupt_handler(74, hal_interrupt_handler_74);
    hal_set_interrupt_handler(75, hal_interrupt_handler_75);
    hal_set_interrupt_handler(76, hal_interrupt_handler_76);
    hal_set_interrupt_handler(77, hal_interrupt_handler_77);
    hal_set_interrupt_handler(78, hal_interrupt_handler_78);
    hal_set_interrupt_handler(79, hal_interrupt_handler_79);
    hal_set_interrupt_handler(80, hal_interrupt_handler_80);
    hal_set_interrupt_handler(81, hal_interrupt_handler_81);
    hal_set_interrupt_handler(82, hal_interrupt_handler_82);
    hal_set_interrupt_handler(83, hal_interrupt_handler_83);
    hal_set_interrupt_handler(84, hal_interrupt_handler_84);
    hal_set_interrupt_handler(85, hal_interrupt_handler_85);
    hal_set_interrupt_handler(86, hal_interrupt_handler_86);
    hal_set_interrupt_handler(87, hal_interrupt_handler_87);
    hal_set_interrupt_handler(88, hal_interrupt_handler_88);
    hal_set_interrupt_handler(89, hal_interrupt_handler_89);
    hal_set_interrupt_handler(90, hal_interrupt_handler_90);
    hal_set_interrupt_handler(91, hal_interrupt_handler_91);
    hal_set_interrupt_handler(92, hal_interrupt_handler_92);
    hal_set_interrupt_handler(93, hal_interrupt_handler_93);
    hal_set_interrupt_handler(94, hal_interrupt_handler_94);
    hal_set_interrupt_handler(95, hal_interrupt_handler_95);
    hal_set_interrupt_handler(96, hal_interrupt_handler_96);
    hal_set_interrupt_handler(97, hal_interrupt_handler_97);
    hal_set_interrupt_handler(98, hal_interrupt_handler_98);
    hal_set_interrupt_handler(99, hal_interrupt_handler_99);
    hal_set_interrupt_handler(100, hal_interrupt_handler_100);
    hal_set_interrupt_handler(101, hal_interrupt_handler_101);
    hal_set_interrupt_handler(102, hal_interrupt_handler_102);
    hal_set_interrupt_handler(103, hal_interrupt_handler_103);
    hal_set_interrupt_handler(104, hal_interrupt_handler_104);
    hal_set_interrupt_handler(105, hal_interrupt_handler_105);
    hal_set_interrupt_handler(106, hal_interrupt_handler_106);
    hal_set_interrupt_handler(107, hal_interrupt_handler_107);
    hal_set_interrupt_handler(108, hal_interrupt_handler_108);
    hal_set_interrupt_handler(109, hal_interrupt_handler_109);
    hal_set_interrupt_handler(110, hal_interrupt_handler_110);
    hal_set_interrupt_handler(111, hal_interrupt_handler_111);
    hal_set_interrupt_handler(112, hal_interrupt_handler_112);
    hal_set_interrupt_handler(113, hal_interrupt_handler_113);
    hal_set_interrupt_handler(114, hal_interrupt_handler_114);
    hal_set_interrupt_handler(115, hal_interrupt_handler_115);
    hal_set_interrupt_handler(116, hal_interrupt_handler_116);
    hal_set_interrupt_handler(117, hal_interrupt_handler_117);
    hal_set_interrupt_handler(118, hal_interrupt_handler_118);
    hal_set_interrupt_handler(119, hal_interrupt_handler_119);
    hal_set_interrupt_handler(120, hal_interrupt_handler_120);
    hal_set_interrupt_handler(121, hal_interrupt_handler_121);
    hal_set_interrupt_handler(122, hal_interrupt_handler_122);
    hal_set_interrupt_handler(123, hal_interrupt_handler_123);
    hal_set_interrupt_handler(124, hal_interrupt_handler_124);
    hal_set_interrupt_handler(125, hal_interrupt_handler_125);
    hal_set_interrupt_handler(126, hal_interrupt_handler_126);
    hal_set_interrupt_handler(127, hal_interrupt_handler_127);
    hal_set_interrupt_handler(128, hal_interrupt_handler_128);
    hal_set_interrupt_handler(129, hal_interrupt_handler_129);
    hal_set_interrupt_handler(130, hal_interrupt_handler_130);
    hal_set_interrupt_handler(131, hal_interrupt_handler_131);
    hal_set_interrupt_handler(132, hal_interrupt_handler_132);
    hal_set_interrupt_handler(133, hal_interrupt_handler_133);
    hal_set_interrupt_handler(134, hal_interrupt_handler_134);
    hal_set_interrupt_handler(135, hal_interrupt_handler_135);
    hal_set_interrupt_handler(136, hal_interrupt_handler_136);
    hal_set_interrupt_handler(137, hal_interrupt_handler_137);
    hal_set_interrupt_handler(138, hal_interrupt_handler_138);
    hal_set_interrupt_handler(139, hal_interrupt_handler_139);
    hal_set_interrupt_handler(140, hal_interrupt_handler_140);
    hal_set_interrupt_handler(141, hal_interrupt_handler_141);
    hal_set_interrupt_handler(142, hal_interrupt_handler_142);
    hal_set_interrupt_handler(143, hal_interrupt_handler_143);
    hal_set_interrupt_handler(144, hal_interrupt_handler_144);
    hal_set_interrupt_handler(145, hal_interrupt_handler_145);
    hal_set_interrupt_handler(146, hal_interrupt_handler_146);
    hal_set_interrupt_handler(147, hal_interrupt_handler_147);
    hal_set_interrupt_handler(148, hal_interrupt_handler_148);
    hal_set_interrupt_handler(149, hal_interrupt_handler_149);
    hal_set_interrupt_handler(150, hal_interrupt_handler_150);
    hal_set_interrupt_handler(151, hal_interrupt_handler_151);
    hal_set_interrupt_handler(152, hal_interrupt_handler_152);
    hal_set_interrupt_handler(153, hal_interrupt_handler_153);
    hal_set_interrupt_handler(154, hal_interrupt_handler_154);
    hal_set_interrupt_handler(155, hal_interrupt_handler_155);
    hal_set_interrupt_handler(156, hal_interrupt_handler_156);
    hal_set_interrupt_handler(157, hal_interrupt_handler_157);
    hal_set_interrupt_handler(158, hal_interrupt_handler_158);
    hal_set_interrupt_handler(159, hal_interrupt_handler_159);
    hal_set_interrupt_handler(160, hal_interrupt_handler_160);
    hal_set_interrupt_handler(161, hal_interrupt_handler_161);
    hal_set_interrupt_handler(162, hal_interrupt_handler_162);
    hal_set_interrupt_handler(163, hal_interrupt_handler_163);
    hal_set_interrupt_handler(164, hal_interrupt_handler_164);
    hal_set_interrupt_handler(165, hal_interrupt_handler_165);
    hal_set_interrupt_handler(166, hal_interrupt_handler_166);
    hal_set_interrupt_handler(167, hal_interrupt_handler_167);
    hal_set_interrupt_handler(168, hal_interrupt_handler_168);
    hal_set_interrupt_handler(169, hal_interrupt_handler_169);
    hal_set_interrupt_handler(170, hal_interrupt_handler_170);
    hal_set_interrupt_handler(171, hal_interrupt_handler_171);
    hal_set_interrupt_handler(172, hal_interrupt_handler_172);
    hal_set_interrupt_handler(173, hal_interrupt_handler_173);
    hal_set_interrupt_handler(174, hal_interrupt_handler_174);
    hal_set_interrupt_handler(175, hal_interrupt_handler_175);
    hal_set_interrupt_handler(176, hal_interrupt_handler_176);
    hal_set_interrupt_handler(177, hal_interrupt_handler_177);
    hal_set_interrupt_handler(178, hal_interrupt_handler_178);
    hal_set_interrupt_handler(179, hal_interrupt_handler_179);
    hal_set_interrupt_handler(180, hal_interrupt_handler_180);
    hal_set_interrupt_handler(181, hal_interrupt_handler_181);
    hal_set_interrupt_handler(182, hal_interrupt_handler_182);
    hal_set_interrupt_handler(183, hal_interrupt_handler_183);
    hal_set_interrupt_handler(184, hal_interrupt_handler_184);
    hal_set_interrupt_handler(185, hal_interrupt_handler_185);
    hal_set_interrupt_handler(186, hal_interrupt_handler_186);
    hal_set_interrupt_handler(187, hal_interrupt_handler_187);
    hal_set_interrupt_handler(188, hal_interrupt_handler_188);
    hal_set_interrupt_handler(189, hal_interrupt_handler_189);
    hal_set_interrupt_handler(190, hal_interrupt_handler_190);
    hal_set_interrupt_handler(191, hal_interrupt_handler_191);
    hal_set_interrupt_handler(192, hal_interrupt_handler_192);
    hal_set_interrupt_handler(193, hal_interrupt_handler_193);
    hal_set_interrupt_handler(194, hal_interrupt_handler_194);
    hal_set_interrupt_handler(195, hal_interrupt_handler_195);
    hal_set_interrupt_handler(196, hal_interrupt_handler_196);
    hal_set_interrupt_handler(197, hal_interrupt_handler_197);
    hal_set_interrupt_handler(198, hal_interrupt_handler_198);
    hal_set_interrupt_handler(199, hal_interrupt_handler_199);
    hal_set_interrupt_handler(200, hal_interrupt_handler_200);
    hal_set_interrupt_handler(201, hal_interrupt_handler_201);
    hal_set_interrupt_handler(202, hal_interrupt_handler_202);
    hal_set_interrupt_handler(203, hal_interrupt_handler_203);
    hal_set_interrupt_handler(204, hal_interrupt_handler_204);
    hal_set_interrupt_handler(205, hal_interrupt_handler_205);
    hal_set_interrupt_handler(206, hal_interrupt_handler_206);
    hal_set_interrupt_handler(207, hal_interrupt_handler_207);
    hal_set_interrupt_handler(208, hal_interrupt_handler_208);
    hal_set_interrupt_handler(209, hal_interrupt_handler_209);
    hal_set_interrupt_handler(210, hal_interrupt_handler_210);
    hal_set_interrupt_handler(211, hal_interrupt_handler_211);
    hal_set_interrupt_handler(212, hal_interrupt_handler_212);
    hal_set_interrupt_handler(213, hal_interrupt_handler_213);
    hal_set_interrupt_handler(214, hal_interrupt_handler_214);
    hal_set_interrupt_handler(215, hal_interrupt_handler_215);
    hal_set_interrupt_handler(216, hal_interrupt_handler_216);
    hal_set_interrupt_handler(217, hal_interrupt_handler_217);
    hal_set_interrupt_handler(218, hal_interrupt_handler_218);
    hal_set_interrupt_handler(219, hal_interrupt_handler_219);
    hal_set_interrupt_handler(220, hal_interrupt_handler_220);
    hal_set_interrupt_handler(221, hal_interrupt_handler_221);
    hal_set_interrupt_handler(222, hal_interrupt_handler_222);
    hal_set_interrupt_handler(223, hal_interrupt_handler_223);
    hal_set_interrupt_handler(224, hal_interrupt_handler_224);
    hal_set_interrupt_handler(225, hal_interrupt_handler_225);
    hal_set_interrupt_handler(226, hal_interrupt_handler_226);
    hal_set_interrupt_handler(227, hal_interrupt_handler_227);
    hal_set_interrupt_handler(228, hal_interrupt_handler_228);
    hal_set_interrupt_handler(229, hal_interrupt_handler_229);
    hal_set_interrupt_handler(230, hal_interrupt_handler_230);
    hal_set_interrupt_handler(231, hal_interrupt_handler_231);
    hal_set_interrupt_handler(232, hal_interrupt_handler_232);
    hal_set_interrupt_handler(233, hal_interrupt_handler_233);
    hal_set_interrupt_handler(234, hal_interrupt_handler_234);
    hal_set_interrupt_handler(235, hal_interrupt_handler_235);
    hal_set_interrupt_handler(236, hal_interrupt_handler_236);
    hal_set_interrupt_handler(237, hal_interrupt_handler_237);
    hal_set_interrupt_handler(238, hal_interrupt_handler_238);
    hal_set_interrupt_handler(239, hal_interrupt_handler_239);
    hal_set_interrupt_handler(240, hal_interrupt_handler_240);
    hal_set_interrupt_handler(241, hal_interrupt_handler_241);
    hal_set_interrupt_handler(242, hal_interrupt_handler_242);
    hal_set_interrupt_handler(243, hal_interrupt_handler_243);
    hal_set_interrupt_handler(244, hal_interrupt_handler_244);
    hal_set_interrupt_handler(245, hal_interrupt_handler_245);
    hal_set_interrupt_handler(246, hal_interrupt_handler_246);
    hal_set_interrupt_handler(247, hal_interrupt_handler_247);
    hal_set_interrupt_handler(248, hal_interrupt_handler_248);
    hal_set_interrupt_handler(249, hal_interrupt_handler_249);
    hal_set_interrupt_handler(250, hal_interrupt_handler_250);
    hal_set_interrupt_handler(251, hal_interrupt_handler_251);
    hal_set_interrupt_handler(252, hal_interrupt_handler_252);
    hal_set_interrupt_handler(253, hal_interrupt_handler_253);
    hal_set_interrupt_handler(254, hal_interrupt_handler_254);
    hal_set_interrupt_handler(255, hal_interrupt_handler_255);
    return;
}

uint32_t KABI hal_get_core_id(void)
{
    return 0;
}

int32_t KABI hal_interrupt_init(void)
{
    uint32_t coreid = hal_get_core_id();
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    eax = 1;
    hal_cpuid(&eax, &ebx, &ecx, &edx);
    if (!(edx & lb_bit_mask(9)))
    {
        hal_printf("ERROR: APIC not supported by CPU.\n");
        return 1;
    }

    // get idt ptr ready
    _idt_ptrs[coreid].base = (uint64_t) &_idts[coreid];
    _idt_ptrs[coreid].limit = IDT_ENTRY_NUM * IDT_ENTRY_SIZE - 1;

    // clear dispatch table
    for (uint64_t i = 0; i < IDT_ENTRY_NUM; i++)
    {
        _intr_handler_table[coreid][i] = NULL;
        _exc_handler_table[coreid][i] = NULL;
        _intr_handler_context_table[coreid][i] = NULL;
    }

    // hook asm interrupt handlers
    _hal_populate_idt();

    hal_flush_idt(&_idt_ptrs[coreid]);

    // disable PIC
    hal_write_port_8(0xa1, 0xff);
    hal_write_port_8(0x21, 0xff);

    uint64_t apic_base_reg = 0;
    uint64_t apic_base = 0;
    ecx = MSR_IA32_APIC_BASE;
    hal_read_msr(&ecx, &edx, &eax);
    apic_base_reg = ((uint64_t) edx << 32) + (uint64_t) eax;
    apic_base = apic_base_reg & lb_bit_field_mask(12, 35);
    //hal_printf("APIC Base: 0x%X\n", apic_base);
    //hal_printf("APIC Enabled: %s\n", apic_base_reg & bit_mask_64(11) ? "Yes" : "No");
    //hal_printf("BSP: %s\n", apic_base_reg & bit_mask_64(8) ? "Yes" : "No");
    //hal_printf("APIC Spour: 0x%X\n", *(uint32_t *) ((char *) apic_base + APIC_SPURIOUS_INT_VEC_REG_OFFSET));
    // hardware enable APIC
    ecx = MSR_IA32_APIC_BASE;
    eax = (uint32_t) ((apic_base_reg & lb_bit_field_mask(0, 31)) | lb_bit_mask(11));
    hal_write_msr(&ecx, &edx, &eax);

    // software enable APIC
    hal_write_mem_32((char *) apic_base + APIC_SPURIOUS_INT_VEC_REG_OFFSET, *(uint32_t *) (apic_base + APIC_SPURIOUS_INT_VEC_REG_OFFSET) | (uint32_t)lb_bit_mask(8));

//    hal_issue_interrupt(1, 255);
//    hal_enable_interrupt();

    return 0;
}