#include "syscall.h"

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
void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}