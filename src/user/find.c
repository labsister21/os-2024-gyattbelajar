#include "find.h"
#include "command.h"
#include "../file-system/fat32.h"

// recursive find
void find_recursive(char* dir_name, char* parent_path, uint32_t parent_cluster, char directories[12][13], int curr_directory_index, int dir_count, bool* haveFound){
    int cluster_number = findEntryName(dir_name);

    if(cluster_number == -1) return;
    updateDirectoryTable(cluster_number);

    char result[256];
    for(int i = 1; i < 64; i++){
        clear(result, 256);
        if(dir_table.table[i].user_attribute == UATTR_NOT_EMPTY){
            str_path_concat(result, parent_path , dir_table.table[i].name);
            int len_compare = 0;
            if(strlen(dir_table.table[i].name) > strlen(directories[curr_directory_index])){
                len_compare = strlen(dir_table.table[i].name);
            } else{
                len_compare = strlen(directories[curr_directory_index]);
            }
            if(strcmp(dir_table.table[i].name, directories[curr_directory_index], len_compare) == 0){
                if(curr_directory_index + 1 == dir_count - 1 ){
                    put(result, BIOS_BROWN);
                    if(dir_table.table[i].attribute == false && dir_table.table[i].ext[0] != '\0'){
                        put(".", BIOS_BROWN );
                        put(dir_table.table[i].ext, BIOS_BROWN);
                    }
                    put("\n", BIOS_BROWN);
                    *haveFound = true;
                }else{
                    find_recursive(dir_table.table[i].name, result, current_directory, directories, 1, dir_count, haveFound);
                }
            } else{
                find_recursive(dir_table.table[i].name, result, current_directory, directories, 0, dir_count, haveFound);
            }
        }
    }

    updateDirectoryTable(parent_cluster);
}


// find command
void find(char (*parsed_args)[128]){
    // update dir_table
    updateDirectoryTable(current_directory);

    char directories[12][13];
    int dir_count = parse_path(parsed_args[1], directories);
    if(dir_count == -1){
        put("find: ", BIOS_RED);
        put(parsed_args[1], BIOS_RED);
        put(": path not valid\n", BIOS_RED);
        return;
    }

    bool haveFound = false;
    for(int i = 1; i < 64; i++){
        if(dir_table.table[i].user_attribute == UATTR_NOT_EMPTY){
            char result[256];
            str_path_concat(result, ".", dir_table.table[i].name);
            int len_compare = 0;
            if(strlen(dir_table.table[i].name) > strlen(directories[0])){
                len_compare = strlen(dir_table.table[i].name);
            } else{
                len_compare = strlen(directories[0]);
            }
            if(strcmp(dir_table.table[i].name, directories[0], len_compare) == 0){
                if(dir_count == 1){
                    put(result, BIOS_BROWN);
                    if(dir_table.table[i].attribute == false && dir_table.table[i].ext[0] != '\0'){
                        put(".", BIOS_BROWN );
                        put(dir_table.table[i].ext, BIOS_BROWN);
                    }
                    put("\n", BIOS_BROWN);
                    haveFound = true;
                }else{
                    find_recursive(dir_table.table[i].name, result, current_directory, directories, 1, dir_count, &haveFound);
                }
            } else{
                find_recursive(dir_table.table[i].name, result, current_directory, directories, 0, dir_count, &haveFound);
            }
        }
    }

    if(!haveFound){
        put("find: ", BIOS_RED);
        put(parsed_args[1], BIOS_RED);
        put(" : No such file or directory\n", BIOS_RED);
    }
}

void print_absolute_path(int current_file) {
    struct FAT32DirectoryTable tempDir;
    syscall(8, (uint32_t) &tempDir, ROOT_CLUSTER_NUMBER, 1);

    char path[6][16];
    int i = 0;
    int current_directory_number = tempDir.table[current_file].parent;
    while (current_directory_number != ROOT_CLUSTER_NUMBER) {
        memcpy(path[i], tempDir.table[current_directory_number].name, 16);
        current_directory_number = tempDir.table[current_directory_number].parent;
        i++;
        if (i >= 10) break; // Safe type
    }

    for (i = 5; i >= 0; i--) {
        if (strlen(path[i]) != 0) {
            put(path[i], BIOS_GREY);
            put("/", BIOS_GREY);
        }
    }
}

void find2(char* filename) {
    struct FAT32DirectoryTable tempDir;
    syscall(8, (uint32_t) &tempDir, ROOT_CLUSTER_NUMBER, 1);

    char name[8];
    int len = strlen(filename);
    int i = 0;
    while (i < len && filename[i] != '.' && i < 8) {
        name[i] = filename[i];
        i++;
    }
    i += 1;
    int k = 0;
    char ext[3];

    while (i < len && k < 3) {
        ext[k] = filename[i];
        k++;
        i++;
    }

    for (i = 0; i < 64; i++) {
        if (strcmp(name, tempDir.table[i].name, 8) == 0 && strcmp(ext, tempDir.table[i].ext, 3) == 0) {
            print_absolute_path(i);
            put(tempDir.table[i].name, BIOS_BROWN);
            int len = strlen(tempDir.table[i].ext);
            if (len != 0) {
                syscall(5, '.', BIOS_LIGHT_BLUE, 0);
                syscall(6, (uint32_t)tempDir.table[i].ext, 3, BIOS_LIGHT_BLUE);
            }
        }
    }
    put("\n", BIOS_BLACK);
}

