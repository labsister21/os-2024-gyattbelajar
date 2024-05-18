#include "ls.h"
#include "../file-system/fat32.h"
#include "command.h"

void ls() {
    int i;
    struct FAT32DirectoryTable tempDir;
    syscall(8, (uint32_t) &tempDir, current_directory, 1);
    // int found 
    for (i = 0; i < 64; i++) {
        // Warna abu untuk folder, biru untuk file
        if (tempDir.table[i].user_attribute == UATTR_NOT_EMPTY && tempDir.table[i].parent == current_directory) {
            // File, attribute = 0
            if (tempDir.table[i].attribute == 0) {
                syscall(6, (uint32_t)tempDir.table[i].name, 8, BIOS_LIGHT_BLUE);
                int len = strlen(tempDir.table[i].ext);
                if (len != 0) {
                    syscall(5, '.', BIOS_LIGHT_BLUE, 0);
                    syscall(6, (uint32_t)tempDir.table[i].ext, 3, BIOS_LIGHT_BLUE);
                }
            } else {
                // Folder, attribute = 1
                syscall(6, (uint32_t)tempDir.table[i].name, 8, BIOS_GREY);
            }
            syscall(5, ' ', BIOS_GREY, 0);
        }
    }
    put("\n", BIOS_GREY);
}