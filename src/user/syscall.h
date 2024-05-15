#include <stdint.h>

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
void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) ;