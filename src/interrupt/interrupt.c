#include "interrupt.h"
#include "../lib-header/portio.h"
#include "../lib-header/keyboard.h"
#include "idt.h"
#include "../lib-header/framebuffer.h"
#include "../file-system/fat32.h"

void main_interrupt_handler(struct InterruptFrame frame) {
    if (frame.int_number < 32) {
        __asm__("hlt");
        return;
    }

    switch (frame.int_number) {
        case 0xd:
            __asm__("hlt");
            break;
        case 0xe: // Page Fault
            __asm__("hlt");
            break;
        case 0x21:
            keyboard_isr();
            break;
        case 0x30:
            syscall(frame);
            break;
    }
}


void io_wait(void) {
    out(0x80, 0);
}

void pic_ack(uint8_t irq) {
    if (irq >= 8) out(PIC2_COMMAND, PIC_ACK);
    out(PIC1_COMMAND, PIC_ACK);
}

void pic_remap(void) {
    // Starts the initialization sequence in cascade mode
    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); 
    io_wait();
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC1_DATA, PIC1_OFFSET); // ICW2: Master PIC vector offset
    io_wait();
    out(PIC2_DATA, PIC2_OFFSET); // ICW2: Slave PIC vector offset
    io_wait();
    out(PIC1_DATA, 0b0100); // ICW3: tell Master PIC, slave PIC at IRQ2 (0000 0100)
    io_wait();
    out(PIC2_DATA, 0b0010); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    out(PIC1_DATA, ICW4_8086);
    io_wait();
    out(PIC2_DATA, ICW4_8086);
    io_wait();

    // Disable all interrupts
    out(PIC1_DATA, PIC_DISABLE_ALL_MASK);
    out(PIC2_DATA, PIC_DISABLE_ALL_MASK);

}

/**
 * System call 
 * 0 : read file
 * 1 : read_directory
 * 2 : write file / folder
 * 3 : delete file / empty folder
 * 4 : get keyboard input
 * 5 : putchar
 * 6 : putstring
 * 7 : active keyboard state
 * 8 : read cluster
 * 9 : clear screen
 */
void syscall(struct InterruptFrame frame) {
    switch (frame.cpu.general.eax) {
        case 0:
            *((int8_t*) frame.cpu.general.ecx) = read(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx
            );
            break;
        case 1:
            *((int8_t*) frame.cpu.general.ecx) = read_directory(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx);
            break;
        case 2:
            *((int8_t*) frame.cpu.general.ecx) = write(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx);
            break;
        case 3:
            *((int8_t*) frame.cpu.general.ecx) = delete(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx);
            break;
        case 4:
            keyboard_state.start_col = frame.cpu.general.ecx;
            keyboard_state_activate();
            __asm__("sti"); // Due IRQ is disabled when main_interrupt_handler() called
            while (is_keyboard_input_on());
            memcpy((char *) frame.cpu.general.ebx, keyboard_state.keyboard_buffer, KEYBOARD_BUFFER_SIZE);
            keyboard_state_deactivate();
            clear(keyboard_state.keyboard_buffer, KEYBOARD_BUFFER_SIZE);
            break;
        case 5:
            putchar((char)(frame.cpu.general.ebx), frame.cpu.general.ecx);
            break;
        case 6:
            puts(
                frame.cpu.general.ebx, 
                frame.cpu.general.ecx, 
                frame.cpu.general.edx
            ); // Assuming puts() exist in kernel
            break;
        case 7: 
            keyboard_state_activate();
            break;
        case 8:
            read_clusters((struct FAT32DirectoryTable*) frame.cpu.general.ebx, frame.cpu.general.ecx, 1);
            break;
        case 9:
            framebuffer_clear();
            // TODO: reset keyboard position
            break;
    }
}



void activate_keyboard_interrupt(void) {
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_KEYBOARD));
}

void deactivate_keyboard_interrupt(void) {
    out(PIC1_DATA, PIC_DISABLE_ALL_MASK);
}

struct TSSEntry _interrupt_tss_entry = {
    .ss0  = GDT_KERNEL_DATA_SEGMENT_SELECTOR,
};

void set_tss_kernel_current_stack(void) {
    uint32_t stack_ptr;
    // Reading base stack frame instead esp
    __asm__ volatile ("mov %%ebp, %0": "=r"(stack_ptr) : /* <Empty> */);
    // Add 8 because 4 for ret address and other 4 is for stack_ptr variable
    _interrupt_tss_entry.esp0 = stack_ptr + 8; 
}

