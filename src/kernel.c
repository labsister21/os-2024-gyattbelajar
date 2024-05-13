#include <stdint.h>
#include <stdbool.h>
#include "lib-header/gdt.h"
#include "lib-header/framebuffer.h"
#include "lib-header/kernel-entrypoint.h"
#include "file-system/disk.h"
#include "interrupt/idt.h"
#include "lib-header/keyboard.h"
#include "interrupt/interrupt.h"
#include "file-system/fat32.h"
#include "lib-header/paging.h"

// void kernel_setup(void) {
//     load_gdt(&_gdt_gdtr);
//     pic_remap();
//     initialize_idt();
//     activate_keyboard_interrupt();
//     framebuffer_clear();
//     framebuffer_set_cursor(0, 0);
//     __asm__("int $0x4");
//     framebuffer_clear();
//     int col = 0;
//     keyboard_state_activate();
//     framebuffer_write(3, 8,  'H', 0, 0xF);
//     framebuffer_write(3, 9,  'a', 0, 0xF);
//     framebuffer_write(3, 10, 'i', 0, 0xF);
//     framebuffer_write(3, 11, '!', 0, 0xF);
//     framebuffer_set_cursor(3, 10);
//     while (true) {
//         keyboard_state_activate();
//         char c;
//          get_keyboard_buffer(&c);
//          if (c) framebuffer_write(0, col++, c, 0xF, 0);
//     }
// }

// void kernel_setup(void) {
//     load_gdt(&_gdt_gdtr);
//     pic_remap();
//     initialize_idt();
//     activate_keyboard_interrupt();
//     framebuffer_clear();
//     framebuffer_set_cursor(0, 0);
//     initialize_filesystem_fat32();
//     gdt_install_tss();
//     set_tss_register();

//     // Allocate first 4 MiB virtual memory
//     paging_allocate_user_page_frame(&_paging_kernel_page_directory, (uint8_t*) 0);

//     // Write shell into memory
//     struct FAT32DriverRequest request = {
//         .buf                   = (uint8_t*) 0,
//         .name                  = "shell",
//         .ext                   = "\0\0\0",
//         .parent_cluster_number = ROOT_CLUSTER_NUMBER,
//         .buffer_size           = 0x100000,
//     };
//     read(request);

//     // Set TSS $esp pointer and jump into shell 
//     set_tss_kernel_current_stack();
//     kernel_execute_user_program((uint8_t*) 0);
//     keyboard_state_activate();

//     while (true) {
//         keyboard_state_activate();
//     }
// }




void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    // keyboard_state_activate();
    initialize_filesystem_fat32();
    gdt_install_tss();
    set_tss_register();

    paging_allocate_user_page_frame(&_paging_kernel_page_directory, (uint8_t*) 0);
// Write shell into memory
    
    // write()
    struct FAT32DriverRequest request = {
        .buf                   = (uint8_t*) 0,
        .name                  = "shell",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0x200000,
    };
    read(request);
    write(request);

    set_tss_kernel_current_stack();
    kernel_execute_user_program((uint8_t*) 0);
    
    while (true) ;
        //  char c;
        //  get_keyboard_buffer(&c);
        //  if (c) framebuffer_write(0, col++, c, 0xF, 0);
    
}
