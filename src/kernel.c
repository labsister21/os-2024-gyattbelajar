#include <stdint.h>
#include <stdbool.h>
#include "lib-header/gdt.h"
#include "lib-header/framebuffer.h"
#include "lib-header/kernel-entrypoint.h"
#include "interrupt/idt.h"
#include "lib-header/keyboard.h"
#include "interrupt/interrupt.h"

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

void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
        
    // int col = 0;
    keyboard_state_activate();
    while (true) {
        keyboard_state_activate();
        //  char c;
        //  get_keyboard_buffer(&c);
        //  if (c) framebuffer_write(0, col++, c, 0xF, 0);
    }
}
