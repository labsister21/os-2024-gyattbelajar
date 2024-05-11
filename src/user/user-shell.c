#include <stdint.h>
#include "../lib-header/string.h"
#include "../file-system/fat32.h"
#include "user-shell.h"

struct CursorPosition CP = {
    .current_length = 0,
};


void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}


void put_template() {
    syscall(6, (uint32_t) "MewingDulu", 11, BIOS_LIGHT_BLUE);
    syscall(6, (uint32_t) "$ ", 3, BIOS_LIGHT_GREEN);
}

void put_template_with_path(char* path) {
    int len = strlen(path);
    syscall(6, (uint32_t) "MewingDulu", 11, BIOS_LIGHT_BLUE);
    syscall(6, (uint32_t) path, len, BIOS_LIGHT_BLUE);
    syscall(6, (uint32_t) "$ ", 3, BIOS_LIGHT_GREEN);
}


int main(void) {
    struct ClusterBuffer      cl[2]   = {0};
    struct FAT32DriverRequest request = {
        .buf                   = &cl,
        .name                  = "shell",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE,
    };
    int32_t retcode;
    syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);
    if (retcode == 0)
        put_template();

    syscall(7, 0, 0, 0);
    char c[2048];
    while (true) {
        syscall(4, (uint32_t) &c, 0, 0); // Get char from keyboard
        // syscall(5, (uint32_t) c, 0xF, 0); // Put char one by one
        
        if (strcmp((char*)c, "ls", strlen("ls")) == 0) {
            syscall(6, (uint32_t) "Calling LS\n", 12, BIOS_RED);
        } else {
            syscall(6, (uint32_t) "Command Tidak Ditemukan!\n", 26, BIOS_RED);
        }
        put_template();
    }

    return 0;
}

