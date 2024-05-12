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
    put("MewingDulu",BIOS_LIGHT_BLUE);
    put("$ ", BIOS_LIGHT_GREEN);
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

int inputparse (char *args_val, char parsed_args[3][128]) {
    int nums = 0;

    // Process to count the args, initialize 0
    int i = 0; // All char index
    int j = 0; // Array index
    int k = 0; // Current array index inside

    // Iterate all over the chars
    // Ignore blanks at first
    while(args_val[i] == ' '){
        i++;
    }
    
    while (args_val[i] != '\0') {
        nums++;
        while (args_val[i] != ' ' && args_val[i] != '\0') {
            parsed_args[j][k] = args_val[i];
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
    char parsed_args[3][128];

    print_starting_screen();

    // Activate keyboard
    syscall(7, 0, 0, 0);

    // listeners
    while (true) {
        // Clear buffer
        clear(args_val, 2048);
        for(int i = 0 ; i < 3; i++){
            clear(parsed_args[i], 128);
        }
        
        // add template
        put_template();
        
        // keyboard input
        syscall(4, (uint32_t) args_val, 2048, 0x0);

        int args_count = inputparse(args_val, parsed_args);

        if(args_count != 0){
            if (strcmp((char*)parsed_args[0], "cd", 2) == 0) {
                // cd command
                if(args_count > 2){
                    put("cd: to many arguments\n", BIOS_RED);
                } else{
                    put("Command cd\n", BIOS_LIGHT_GREEN);
                }

            } else if (strcmp((char*)parsed_args[0], "ls", 2) == 0) {
                // ls command
                ls();

            } else if (strcmp((char*)parsed_args[0], "mkdir", 5) == 0) {
                // mkdir command
                if(args_count < 2){
                    put("mkdir: missing operand\n", BIOS_RED);
                } 
                put("Command mkdir\n", BIOS_LIGHT_GREEN);

            // } else if (strcmp((char*)parsed_args[0], "cat", 3) == 0) {
            //     // cat command
            //     put("Command cat\n", BIOS_LIGHT_GREEN);

            // } else if (strcmp((char*)parsed_args[0], "cp", 2) == 0) {
            //     // cp command
            //     put("Command cp\n", BIOS_LIGHT_GREEN);

            // } else if (strcmp((char*)parsed_args[0], "rm", 2) == 0) {
            //     // rm command
            //     put("Command rm\n", BIOS_LIGHT_GREEN);

            // } else if (strcmp((char*)parsed_args[0], "mv", 2) == 0) {
            //     // mv command
            //     put("Command mv\n", BIOS_LIGHT_GREEN);

            // } else if (strcmp((char*)parsed_args[0], "find", 4) == 0) {
            //     // whereis command
            //     put("Command find", BIOS_LIGHT_GREEN);

            } else if (strcmp((char*)parsed_args[0], "clear", strlen("clear")) == 0) {
                // clear command
                if (args_count > 1) {
                    put("clear: too many arguments\n", BIOS_RED);
                } else {
                    // clear screen
                    // TODO: reset keyboard position
                    syscall(9, 0, 0, 0);
                }

            } else {
                put(args_val, BIOS_RED);
                put(": command not found\n", BIOS_RED);
            }
        }
        
    }

    return 0;
}


