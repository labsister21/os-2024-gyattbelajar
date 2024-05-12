#include "ls.h"
#include "../file-system/fat32.h"
#include "user-shell.h"

void ls() {
    int i;
    syscall(8, (uint32_t) &dir_table, 2, 1);
    for (i = 0; i < 63; i++) {
        if (dir_table.table[i].user_attribute == UATTR_NOT_EMPTY) {
            syscall(6, (uint32_t)dir_table.table[i].name, 8, BIOS_GREY);
            syscall(5, ' ', BIOS_GREY, 0);
        } else {
            break;
        }
    }
    put("\n", BIOS_GREY);
}