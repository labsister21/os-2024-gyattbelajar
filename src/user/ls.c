#include "ls.h"
#include "../file-system/fat32.h"
#include "user-shell.h"

void print_directory() {
    int i;
    for (i = 0; i < 63; i++) {
        if (dir_table.table[i].user_attribute == UATTR_NOT_EMPTY) {
            syscall(6, (uint32_t)dir_table.table[i].name, 7, BIOS_GREY);
        }
    }
}