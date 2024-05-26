#include "rm.h"
#include "command.h"
#include "../file-system/fat32.h"

void rm(char *parsed_args){
    updateDirectoryTable(current_directory);

    char nama[8];
    memset(nama, '\0', 8);
    int len = strlen(parsed_args);
    int i = 0;
    while (i < len && parsed_args[i] != '.' && i < 8) {
        nama[i] = parsed_args[i];
        i++;
    }

    if (current_directory == ROOT_CLUSTER_NUMBER && (strcmp(nama, "root", 5) == 0 || strcmp(nama, "shell", 6) == 0)) {
        put("Tidak bisa menghapus file ini!", BIOS_RED);
        put("\n", BIOS_BLACK);
    }
    i += 1;
    int k = 0;
    char eks[3];
    memset(eks, '\0', 3);

    while (i < len && k < 3) {
        eks[k] = parsed_args[i];
        k++;
        i++;
    }

    updateDirectoryTable(current_directory);
    struct FAT32DirectoryTable tempDir = dir_table;
    tempDir = dir_table; 

    struct FAT32DriverRequest request = {
        .parent_cluster_number = current_directory,
        .buffer_size = CLUSTER_SIZE,
    };
    memcpy(&request.name, nama, 8);
    memcpy(&request.ext, eks, 3);

    bool found = false;
    int lenA = strlen(nama);
    for (i = 0; i < 63; i++) {
        if (strcmp(tempDir.table[i].name, nama, lenA) == 0 && strcmp(tempDir.table[i].ext, eks, 3) == 0) {
            found = true;
            syscall(3, (uint32_t) &request, 0, 0);
            break;
        }
    }
    if (found) {
        put("Berhasil menghapus file ", BIOS_LIGHT_BLUE);
    } else {
        put("File tidak ditemukan: ", BIOS_RED);
    }
    put(nama, BIOS_LIGHT_BLUE);
    put(".", BIOS_LIGHT_BLUE);
    put(eks, BIOS_LIGHT_BLUE);
    put("\n", BIOS_BLACK);
    
    // char directories[12][13];
    // int dir_count = parse_path(parsed_args[1], directories);
    // if(dir_count == -1){
    //     put("rm: cannot remove '", BIOS_RED);
    //     put(parsed_args[1], BIOS_RED);
    //     put("': No such file or director\n", BIOS_RED);
    //     return;
    // }

    // int cluster_number = 0;
    // for(int i = 0; i < dir_count - 1; i++){
    //     // Go to directory
    //     cluster_number = findEntryName(directories[i]);
    //     if(cluster_number == -1){
    //         put("rm: cannot remove '", BIOS_RED);
    //         put(parsed_args[1], BIOS_RED);
    //         put("': No such file or director\n", BIOS_RED);
    //         return;
    //     }

    //     updateDirectoryTable(cluster_number);
    // }

    // int file_cluster = findEntryName(directories[dir_count - 1]);
    // if(file_cluster == -1){
    //     put("rm: cannot remove '", BIOS_RED);
    //     put(parsed_args[1], BIOS_RED);
    //     put("': No such file or director\n", BIOS_RED);
    //     return;
    // }

    // // check if file
    // bool is_file = false;
    // char file_name[8] = "\0\0\0\0\0\0\0\0";
    // char file_ext[3] = "\0\0\0";
    // int k = 0;
    // int file_len = strlen(directories[dir_count - 1]);
    // for(int i = 0; i < file_len; i--){
    //     if(is_file){
    //         file_name[i] = directories[dir_count - 1][i];
    //     } else if(directories[dir_count - 1][i] == '.'){
    //         is_file = true;
    //     } else{
    //         file_ext[k] = directories[dir_count - 1][i];
    //         k++;
    //     }
    // }

    // if(!is_file){
    //     put("rm: cannot remove '", BIOS_RED);
    //     put(parsed_args[1], BIOS_RED);
    //     put("': remove directory is not suported\n", BIOS_RED);
    //     return;
    // }

    // // remove file
    // struct FAT32DriverRequest request = {
    //     .name                  = {0},
    //     .ext                   = {0},
    //     .parent_cluster_number = cluster_number
    // };
    // memcpy(request.name, file_name, 8);
    // memcpy(request.ext, file_ext, 3);
    
    // int retcode = 0;
    // syscall(3, (uint32_t) &request, (uint32_t) &retcode, 0);
    // if(retcode != 0){
    //     put("rm: cannot remove '", BIOS_RED);
    //     put(parsed_args[1], BIOS_RED);
    //     put("': No such file or director\n", BIOS_RED);
    //     return;
    // }

    // // reset dir_table
    // updateDirectoryTable(current_directory);
}