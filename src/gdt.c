#include "lib-header/gdt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        { // null descriptor
            .segment_low = 0,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0,
            .non_system = 0,
            .dpl_privilege = 0,
            .present = 0,
            .segment_high = 0,
            .available_bit = 0,
            .code_in_64 = 0,
            .db_flag = 0,
            .granularity = 0,
            .base_high = 0,
        },
        { // kernel code descriptor
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0xA,
            .non_system = 1,
            .dpl_privilege = 0,
            .present = 1,
            .segment_high = 0xF,
            .available_bit = 0,
            .code_in_64 = 0,
            .db_flag = 1,
            .granularity = 1,
            .base_high = 0,
        },
        { // kernel data descriptor
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0x2,
            .non_system = 1,
            .dpl_privilege = 0,
            .present = 1,
            .segment_high = 0xF,
            .available_bit = 0,
            .code_in_64 = 0,
            .db_flag = 1,
            .granularity = 1,
            .base_high = 0,
        },
        { // user code descriptor
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0xA,
            .non_system = 1,
            .dpl_privilege = 0x3,
            .present = 1,
            .segment_high = 0xF,
            .available_bit = 0,
            .code_in_64 = 0,
            .db_flag = 1,
            .granularity = 1,
            .base_high = 0,
        },
        { // user data descriptor
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0x2,
            .non_system = 0x1,
            .dpl_privilege = 0x3,
            .present = 0x01,
            .segment_high = 0xF,
            .available_bit = 0,
            .code_in_64 = 0,
            .db_flag = 1,
            .granularity = 1,
            .base_high = 0,
        },
        { // TSS
            .segment_low       = sizeof(struct TSSEntry),
            .base_low          = 0,
            .base_mid          = 0,
            .type_bit          = 0x9,
            .non_system        = 0,    // S bit
            .dpl_privilege     = 0,    // DPL
            .present           = 1,    // P bit
            .segment_high      = (sizeof(struct TSSEntry) & (0xF << 16)) >> 16,
            .available_bit     = 0,
            .code_in_64         = 0,    // L bit
            .db_flag          = 1,    // D/B bit
            .granularity       = 0,    // G bit
            .base_high         = 0,
        },
        {0}
        }
        };


void gdt_install_tss(void) {
    uint32_t base = (uint32_t) &_interrupt_tss_entry;
    global_descriptor_table.table[5].base_high = (base & (0xFF << 24)) >> 24;
    global_descriptor_table.table[5].base_mid  = (base & (0xFF << 16)) >> 16;
    global_descriptor_table.table[5].base_low  = base & 0xFFFF;
}


/**
 * _gdt_gdtr, predefined system GDTR.
 * GDT pointed by this variable is already set to point global_descriptor_table above.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    .size = sizeof(global_descriptor_table),
    .address = &global_descriptor_table};
