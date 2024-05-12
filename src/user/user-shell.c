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

/**
 * System call to interupt
 * 0 : read file
 * 1 : read_directory
 * 2 : write file / folder
 * 3 : delete file / empty folder
 * 4 : get keyboard input
 * 5 : putchar
 * 6 : putstring
 * 7 : active keyboard state
 * 8 : read cluster
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

// Wrapper for syscall 6
void put(char* buf, uint8_t color) {
    syscall(6, (uint32_t) buf, strlen(buf), color);
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

void print_starting_screen(){
    // Mewcrosoft ascii art
    // put("$$$$$$\\$$$$\\  $$$$$$\\ $$\\  $$\\  $$\\ $$$$$$$\\ $$$$$$\\  $$$$$$\\  $$$$$$$\\ $$$$$$\\ $$ /  \\__$$$$$$\\   \n", BIOS_LIGHT_GREEN);
    // put("$$  _$$  _$$\\$$  __$$\\$$ | $$ | $$ $$  _____$$  __$$\\$$  __$$\\$$  _____$$  __$$\\$$$$\\    \\_$$  _|  \n", BIOS_LIGHT_GREEN);
    // put("$$ / $$ / $$ $$$$$$$$ $$ | $$ | $$ $$ /     $$ |  \\__$$ /  $$ \\$$$$$$\\ $$ /  $$ $$  _|     $$ |    \n", BIOS_LIGHT_GREEN);
    // put("$$ | $$ | $$ $$   ____$$ | $$ | $$ $$ |     $$ |     $$ |  $$ |\\____$$\\$$ |  $$ $$ |       $$ |$$\\ \n", BIOS_LIGHT_GREEN);
    // put("$$ | $$ | $$ \\$$$$$$$\\\\$$$$$\\$$$$  \\$$$$$$$\\$$ |     \\$$$$$$  $$$$$$$  \\$$$$$$  $$ |       \\$$$$  |\n", BIOS_LIGHT_GREEN);
    // put("\\__| \\__| \\__|\\_______|\\_____|\\____/ \\_______\\__|      \\______/\\_______/ \\______/\\__|        \\____/ \n\n", BIOS_LIGHT_GREEN);
}

int main(void) {
    // Buffer   
    char args_val[2048];
    // int args_info[128][2];
    // char path_str[2048];


    print_starting_screen();

    // Activate keyboard
    syscall(7, 0, 0, 0);

    // listeners
    while (true) {
        // Clear buffer
        clear(args_val, 2048);
        
        // add template
        put_template();

        // keyboard input
        syscall(4, (uint32_t) args_val, 2048, 0x0);
        
        if (strcmp((char*)args_val, "ls", strlen("ls")) == 0) {
            print_directory();
            put("\n", BIOS_RED);
        } else {
            put("Command Tidak Ditemukan!\n", BIOS_RED);
        }   

        // int num_args = inputparse(c, args);
        // if (num_args > 3){}
        
        // Clear all input buffer
        
        
        // clear(args[0], 128);
        // clear(args[1], 128);
        // clear(args[2], 128);
    }

    return 0;
}


