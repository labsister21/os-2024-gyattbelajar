#include "find.h"
#include "command.h"

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
