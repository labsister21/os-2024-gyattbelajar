#include "command.h"

struct CursorPosition CP;

uint32_t current_directory = ROOT_CLUSTER_NUMBER;
struct FAT32DirectoryTable dir_table;


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



void print_starting_screen(){
    // Mewcrosoft ascii art
    // put("$$$$$$\\$$$$\\  $$$$$$\\ $$\\  $$\\  $$\\ $$$$$$$\\ $$$$$$\\  $$$$$$\\  $$$$$$$\\ $$$$$$\\ $$ /  \\__$$$$$$\\   \n", BIOS_LIGHT_GREEN);
    // put("$$  _$$  _$$\\$$  __$$\\$$ | $$ | $$ $$  _____$$  __$$\\$$  __$$\\$$  _____$$  __$$\\$$$$\\    \\_$$  _|  \n", BIOS_LIGHT_GREEN);
    // put("$$ / $$ / $$ $$$$$$$$ $$ | $$ | $$ $$ /     $$ |  \\__$$ /  $$ \\$$$$$$\\ $$ /  $$ $$  _|     $$ |    \n", BIOS_LIGHT_GREEN);
    // put("$$ | $$ | $$ $$   ____$$ | $$ | $$ $$ |     $$ |     $$ |  $$ |\\____$$\\$$ |  $$ $$ |       $$ |$$\\ \n", BIOS_LIGHT_GREEN);
    // put("$$ | $$ | $$ \\$$$$$$$\\\\$$$$$\\$$$$  \\$$$$$$$\\$$ |     \\$$$$$$  $$$$$$$  \\$$$$$$  $$ |       \\$$$$  |\n", BIOS_LIGHT_GREEN);
    // put("\\__| \\__| \\__|\\_______|\\_____|\\____/ \\_______\\__|      \\______/\\_______/ \\______/\\__|        \\____/ \n\n", BIOS_LIGHT_GREEN);
}

void start_command() {
    char args_val[2048];
    char parsed_args[3][128];
    while (true) {
        // Clear buffer
        clear(args_val, 2048);
        for(int i = 0 ; i < 3; i++){
            clear(parsed_args[i], 128);
        }
        
        // add template
        put_template();
        
        // keyboard input
        syscall(4, (uint32_t) args_val, CP.start_col, 0x0);

        int args_count = inputparse(args_val, parsed_args);

        if(args_count != 0){
            if (strcmp((char*)parsed_args[0], "cd", 3) == 0) {
                // cd command
                if(args_count > 2){
                    put("cd: to many arguments\n", BIOS_RED);
                } else{
                    put("Command cd\n", BIOS_LIGHT_GREEN);
                }

            } else if (strcmp((char*)parsed_args[0], "ls", 3) == 0) {
                // ls command
                // ls();

            } else if (strcmp((char*)parsed_args[0], "mkdir", 6) == 0) {
                // mkdir command
                if(args_count < 2){
                    put("mkdir: missing operand\n", BIOS_RED);
                } 
                put("Command mkdir\n", BIOS_LIGHT_GREEN);

            } else if (strcmp((char*)parsed_args[0], "cat", 4) == 0) {
                // cat command
                put("Command cat\n", BIOS_LIGHT_GREEN);

            } else if (strcmp((char*)parsed_args[0], "cp", 3) == 0) {
                // cp command
                put("Command cp\n", BIOS_LIGHT_GREEN);

            } else if (strcmp((char*)parsed_args[0], "rm", 3) == 0) {
                // rm command
                put("Command rm\n", BIOS_LIGHT_GREEN);

            } else if (strcmp((char*)parsed_args[0], "mv", 3) == 0) {
                // mv command
                put("Command mv\n", BIOS_LIGHT_GREEN);

            } else if (strcmp((char*)parsed_args[0], "find", 5) == 0) {
                // whereis command
                put("Command find\n", BIOS_LIGHT_GREEN);

            } else if (strcmp((char*)parsed_args[0], "clear", 6) == 0) {
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

}