#include "find.h"
#include "command.h"

// recursive find
void find_recursive(char* dir_name, char* parent_path, int parent_cluster){
    int cluster_number = findEntryName(dir_name);
    if(cluster_number == -1) return;
    updateDirectoryTable(cluster_number);

    // i = 0 --> curr_dir
    for (int i = 1; i < 63; i++) {
        if (dir_table.table[i].user_attribute == UATTR_NOT_EMPTY) {
            char result[256];
            str_path_concat(result, parent_path, dir_table.table[i].name);

            put(result, BIOS_BROWN);
            
            // print extention for files
            if(dir_table.table[i].ext[0] != '\0'){
                put(".", BIOS_BROWN);
                put(dir_table.table[i].ext, BIOS_BROWN);
            } else{
                find_recursive(dir_table.table[i].name, result, cluster_number);
            }

            put("\n", BIOS_GREY);
        } else {
            break;
        }
    }

    // reset dir_table
    updateDirectoryTable(parent_cluster);
}


// find command
void find(char (*parsed_arg)[128], int args_count){
    // update dir_table
    updateDirectoryTable(current_directory);

    uint32_t temp_current_directory = current_directory;

    if(args_count == 1){
        for (int i = 1; i < 63; i++) {
            if(dir_table.table[i].user_attribute == UATTR_NOT_EMPTY){
                put("./", BIOS_BROWN);
                put(dir_table.table[i].name, BIOS_BROWN);
                put("\n", BIOS_BROWN);
                find_recursive(dir_table.table[i].name, "./", current_directory);
            }
        }

    } else{
        char directories[12][13];
        int dir_count = parse_path(parsed_arg[1], directories);
        if(dir_count == -1){
            put("find: ", BIOS_RED);
            put(parsed_arg[1], BIOS_RED);
            put(": No such file or directory\n", BIOS_RED);
            return;
        }
        
        
        for(int i = 0; i < dir_count; i++){
            int cluster_number = findEntryName(directories[i]);
            if(cluster_number == -1){
                put("find: ", BIOS_RED);
                put(parsed_arg[1], BIOS_RED);
                put(": No such file or directory\n", BIOS_RED);
                return;
                
            } else { 
                // go inside the director
                updateDirectoryTable(cluster_number);
            }

            if(i == dir_count - 1){
                // j = 0 --> curr_dir
                put(parsed_arg[1], BIOS_BROWN);
                put("\n", BIOS_BROWN);
                for (int j = 1; j < 63; j++) {
                    if (dir_table.table[j].user_attribute == UATTR_NOT_EMPTY) {
                        char result[256];
                        str_path_concat(result, parsed_arg[1], dir_table.table[j].name);

                        put(result, BIOS_BROWN);
                        
                        // print extention for files
                        if(dir_table.table[j].ext[0] != '\0'){
                            put(".", BIOS_BROWN);
                            put(dir_table.table[j].ext, BIOS_BROWN);
                        } else{
                            find_recursive(dir_table.table[j].name, result, cluster_number);
                        }

                        put("\n", BIOS_BROWN);
                    } else {
                        break;
                    }
                }
            }
        }
    }

    // reset dir_table
    updateDirectoryTable(current_directory);
}
