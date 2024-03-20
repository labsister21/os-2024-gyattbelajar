#include <stdint.h>
#include <stdbool.h>
#include "lib-header/gdt.h"
#include "lib-header/framebuffer.h"
#include "lib-header/kernel-entrypoint.h"

void kernel_setup(void) {
    framebuffer_clear();
    framebuffer_write(3, 8,  'H', 0, 0xF);
    framebuffer_write(3, 9,  'a', 0, 0xF);
    framebuffer_write(3, 10, 'i', 0, 0xF);
    framebuffer_write(3, 11, '!', 0, 0xF);
    framebuffer_set_cursor(3, 10);
    while (true);
}