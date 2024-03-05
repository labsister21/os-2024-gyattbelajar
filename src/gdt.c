#include "header/cpu/gdt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        {
            0;
        },
        {
            .segment_low = 0;
            .base_low = 0;
            .base_mid = 0;
            .type_bit = 0b1010;
            .non_system = 1;
            .dpl = 0;
            .segment_present = 1;
            .seg_limit = 0;
            .avl: 1;
            .code_segment: 1;
            .def_operation_size: 1;
            .granularity: 1;
            .base_high;
        },
        {
            // TODO : Implement
        }
    }
};

/**
 * _gdt_gdtr, predefined system GDTR. 
 * GDT pointed by this variable is already set to point global_descriptor_table above.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    // TODO : Implement, this GDTR will point to global_descriptor_table. 
    //        Use sizeof operator
};
