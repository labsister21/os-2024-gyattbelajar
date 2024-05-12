#include <stdint.h>
#include "../lib-header/string.h"
#include "../file-system/fat32.h"
#include "user-shell.h"

struct CursorPosition CP = {
    .current_length = 0,
    .start_col = 0,
};

uint32_t current_directory = ROOT_CLUSTER_NUMBER;
struct FAT32DirectoryTable dir_table;

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
    CP.start_col = 12;
}

void put_template_with_path(char* path) {
    int len = strlen(path);
    syscall(6, (uint32_t) "MewingDulu", 11, BIOS_LIGHT_BLUE);
    syscall(6, (uint32_t) path, len, BIOS_LIGHT_BLUE);
    syscall(6, (uint32_t) "$ ", 3, BIOS_LIGHT_GREEN);
    CP.start_col = 12 + len - 1;
}

int inputparse (char *input, char parsed_args[3][128]) {
    // Declare the vars
    int nums = -1;
    // int len = strlen(input);

    // Process to count the args, initialize 0
    int i = 0; // All char index
    int j = 0; // Array index
    int k = 0; // Current array index inside

    // Iterate all over the chars
    // Ignore blanks at first
    while (input[i] != '\0') {
        nums++;
        while (input[i] != ' ') {
            parsed_args[j][k] = input[i];
            k++;
            i++;
        }
        parsed_args[j][k] = '\0';
        j++;
        i++;
        k = 0;
    }
    return nums;
}

int main(void) {
    // Insert into shell
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
    
    // char path_str[128];

    syscall(7, 0, 0, 0);
    char c[2048];
    // char args[3][128];
    while (true) {
        syscall(4, (uint32_t) &c, (uint32_t) CP.start_col, 0); // Get char from keyboard
        // syscall(5, (uint32_t) c, 0xF, 0); // Put char one by one
        
        if (strcmp((char*)c, "ls", strlen("ls")) == 0) {
            syscall(6, (uint32_t) "Calling LS\n", 12, BIOS_RED);
            print_directory();
        } else {
            syscall(6, (uint32_t) "Command Tidak Ditemukan!\n", 26, BIOS_RED);
        }

        // int num_args = inputparse(c, args);
        // if (num_args > 3){}
        
        // Clear all input buffer
        clear(c, 2048);
        // clear(args[0], 128);
        // clear(args[1], 128);
        // clear(args[2], 128);
        put_template();
    }

    return 0;
}


