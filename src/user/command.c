#include "command.h"
#include "find.h"

struct CursorPosition CP;

uint32_t current_directory = ROOT_CLUSTER_NUMBER;
struct FAT32DirectoryTable dir_table;


int inputparse (char *args_val, char parsed_args[5][128]) {
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

// parse argument to paths
// return -1 if directory name length is > 13
// doesn't check if the directory is valid
// return amount of directory
int parse_path(char *path, char directories[12][13]) {
    int currIdx = 0;
    int endIdx = strlen(path) - 1;
    char temp_dir[13] = {0};
    int name_len = 0;
    int dir_count = 0;

    while (currIdx <= endIdx) {
        if (path[currIdx] == '/') {
            if (name_len > 0) { // Only add if there was a name
                memcpy(directories[dir_count], temp_dir, name_len);
                clear(temp_dir, 13);
                name_len = 0;
                dir_count++;
                
            } else if(currIdx > 1){
                // path not valid
                return -1;  
            }
        } else {
            temp_dir[name_len] = path[currIdx];
            name_len++;
            if (name_len > 13) {
                // name + . + extension
                // name limit is 8
                // extension limit is 3
                // total len is 13
                return -1;
            }
        }
        currIdx++;
    }

    if (name_len > 0) { // Add the last directory if exists
        memcpy(directories[dir_count], temp_dir, name_len);
        dir_count++;
    }

    return dir_count;
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

// Check if path is absolute
bool is_path_absolute(char (*args_value)){
    return memcmp(args_value, "/", 1) == 0;
}

// Find the name of directory in the dir_table and return its cluster number
int findEntryName(char* name) {
    // i = 0 --> curr_dir
    int i = 1;
    while (i < 64) {
        int name_len = strlen(name);
        int dir_name_len = strlen(dir_table.table[i].name);

        if(name_len >= dir_name_len){
            if (memcmp(dir_table.table[i].name, name, name_len) == 0 && 
                dir_table.table[i].user_attribute == UATTR_NOT_EMPTY) 
                return i;

        } else {
            if (memcmp(dir_table.table[i].name, name, dir_name_len) == 0 && 
                dir_table.table[i].user_attribute == UATTR_NOT_EMPTY) 
                return i;
        }
    
        i++;
    }

    return -1;
}

// Update the dir_table according to the cluster number
void updateDirectoryTable(uint32_t cluster_number) {
    // read cluster
    syscall(8, (uint32_t) &dir_table, cluster_number, 0x0);
}

void print_starting_screen(){
    // Mewcrosoft ascii art
    put("================================================================================\n", BIOS_DARK_GREY);
    put("                      MEWCROSOFT\n\n", BIOS_LIGHT_GREEN);
    put("                  Made By GyattBelajar\n", BIOS_WHITE);
    put("   https://github.com/labsister21/os-2024-gyattbelajar\n", BIOS_WHITE);
    put("                    Mewwing all day\n\n", BIOS_RED);
    put("================================================================================\n", BIOS_DARK_GREY);
}

void start_command() {
    char args_val[2048];
    char parsed_args[5][128];
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
                if (args_count > 2){
                    put("find: too many arguments\n", BIOS_RED);
                } else if(args_count < 2){
                    put("find: missing operand\n", BIOS_RED);
                } else{
                    find(parsed_args);
                }

            } else if (strcmp((char*)parsed_args[0], "clear", 6) == 0) {
                // clear command
                if (args_count > 1) {
                    put("clear: too many arguments\n", BIOS_RED);
                } else {
                    // clear screen
                    syscall(9, 0, 0, 0);
                }

            } else {
                put(args_val, BIOS_RED);
                put(": command not found\n", BIOS_RED);
            }
        }
        
    }

}