#include "rm.h"
#include "command.h"
#include "../file-system/fat32.h"

void rm(char (*parsed_args)[128]){
    updateDirectoryTable(current_directory);
    
    char directories[12][13];
    int dir_count = parse_path(parsed_args[1], directories);
    if(dir_count == -1){
        put("rm: cannot remove '", BIOS_RED);
        put(parsed_args[1], BIOS_RED);
        put("': No such file or director\n", BIOS_RED);
        return;
    }

    int cluster_number = 0;
    for(int i = 0; i < dir_count - 1; i++){
        // Go to directory
        cluster_number = findEntryName(directories[i]);
        if(cluster_number == -1){
            put("rm: cannot remove '", BIOS_RED);
            put(parsed_args[1], BIOS_RED);
            put("': No such file or director\n", BIOS_RED);
            return;
        }

        updateDirectoryTable(cluster_number);
    }

    int file_cluster = findEntryName(directories[dir_count - 1]);
    if(file_cluster == -1){
        put("rm: cannot remove '", BIOS_RED);
        put(parsed_args[1], BIOS_RED);
        put("': No such file or director\n", BIOS_RED);
        return;
    }

    // check if file
    bool is_file = false;
    char file_name[8] = "\0\0\0\0\0\0\0\0";
    char file_ext[3] = "\0\0\0";
    int k = 0;
    int file_len = strlen(directories[dir_count - 1]);
    for(int i = 0; i < file_len; i--){
        if(is_file){
            file_name[i] = directories[dir_count - 1][i];
        } else if(directories[dir_count - 1][i] == '.'){
            is_file = true;
        } else{
            file_ext[k] = directories[dir_count - 1][i];
            k++;
        }
    }

    if(!is_file){
        put("rm: cannot remove '", BIOS_RED);
        put(parsed_args[1], BIOS_RED);
        put("': remove directory is not suported\n", BIOS_RED);
        return;
    }

    // remove file
    struct FAT32DriverRequest request = {
        .name                  = {0},
        .ext                   = {0},
        .parent_cluster_number = cluster_number
    };
    memcpy(request.name, file_name, 8);
    memcpy(request.ext, file_ext, 3);
    
    int retcode = 0;
    syscall(3, (uint32_t) &request, (uint32_t) &retcode, 0);
    if(retcode != 0){
        put("rm: cannot remove '", BIOS_RED);
        put(parsed_args[1], BIOS_RED);
        put("': No such file or director\n", BIOS_RED);
        return;
    }

    // reset dir_table
    updateDirectoryTable(current_directory);
}