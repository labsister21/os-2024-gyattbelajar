#include "mv.h"
#include "command.h"
#include "../file-system/fat32.h"

// move and rename file
// parsed_args[1] -> source
// parsed_args[2] -> destination
void mv(char (*parsed_args)[128]){
    updateDirectoryTable(current_directory);

    // Find Source File
    char src_directories[12][13];
    // char src_name[8];
    // char src_ext[3];
    int src_dir_count = parse_path(parsed_args[1], src_directories);
    if(src_dir_count == -1){
        put("cp: ", BIOS_RED);
        put(parsed_args[1], BIOS_RED);
        put(": source path not valid\n", BIOS_RED);
        return;
    }

    // int src_cluster_number = 0;

    // // Go to source directory
    // for(int i = 0; i < src_dir_count - 1; i++){
    //     src_cluster_number = findEntryName(src_directories[0]);
    //     if(src_cluster_number == -1){
    //         put("cp: ", BIOS_RED);
    //         put(parsed_args[1], BIOS_RED);
    //         put(": No such file or directory\n", BIOS_RED);
    //         return;
    //     }

    //     // change dir_table
    //     updateDirectoryTable(src_cluster_number);
    // }

    // // Find source file
    // bool found = false;
    // for(int i = 1; i < 64; i++){
    //     if(dir_table.table[i].user_attribute == UATTR_NOT_EMPTY){
    //         put("Changed directory\n", BIOS_LIGHT_BLUE);

    //         // check file name
    //         if(strcmp(dir_table.table[i].name, src_directories[src_dir_count - 1], 8) == 0){
                
    //             // check extenstion
    //             bool is_ext = false;
    //             int k = 0;
    //             for(unsigned j = 0; j < strlen(src_directories[src_dir_count - 1]); j++){
    //                 if(is_ext){
    //                     src_ext[k] = src_directories[src_dir_count - 1][j];
    //                     k++;
    //                 } else if(src_directories[src_dir_count - 1][j] == '.'){
    //                     is_ext = true;
    //                 } else{
    //                     src_name[j] = src_directories[src_dir_count - 1][j];
    //                 }
    //             }
                
    //             if(strcmp(dir_table.table[i].ext, src_ext, 3) == 0){
    //                 found = true;
    //                 break;
    //             } 
    //         }
    //     }
    // }

    // if(!found){
    //     put("cp: ", BIOS_RED);
    //     put(parsed_args[1], BIOS_RED);
    //     put(": No such file or directory\n", BIOS_RED);
    //     return;
    // }    

    // // reset directory
    // updateDirectoryTable(current_directory);

    // // Go to destination directory
    // char dest_directories[12][13];
    // char dest_name[8];
    // char dest_ext[3];
    // int dest_dir_count = parse_path(parsed_args[2], dest_directories);
    // int dest_cluster_number = 0;
    // if(dest_dir_count == -1){
    //     put("cp: ", BIOS_RED);
    //     put(parsed_args[2], BIOS_RED);
    //     put(": destination path not valid\n", BIOS_RED);
    //     return;
    // }

    // for(int i = 0; i < dest_dir_count - 1; i++){
    //     dest_cluster_number = findEntryName(dest_directories[0]);
    //     if(dest_cluster_number == -1){
    //         put("cp: ", BIOS_RED);
    //         put(parsed_args[2], BIOS_RED);
    //         put(": No such file or directory\n", BIOS_RED);
    //         return;
    //     }
        
    //     // change dir_table
    //     updateDirectoryTable(dest_cluster_number);
    // }

    // // read file
    // struct ClusterBuffer      cl      = {0};
    // struct FAT32DriverRequest request = {
    //     .buf                   = &cl,
    //     .name                  = {0},
    //     .ext                   = {0},
    //     .parent_cluster_number = src_cluster_number,
    //     .buffer_size           = 0x100000,
    // };
    // memcpy(request.name, src_name, 8);
    // memcpy(request.ext, src_ext, 3);
    
    // int retcode;
    // syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);
    // if(retcode != 0){
    //     put("cp: ", BIOS_RED);
    //     put(parsed_args[2], BIOS_RED);
    //     put(": Read failed\n", BIOS_RED);
    //     return;
    // }

    // // write file
    // request.parent_cluster_number = dest_cluster_number;
    // syscall(2, (uint32_t) &request, (uint32_t) &retcode, 0);
    // if(retcode != 0){
    //     put("cp: ", BIOS_RED);
    //     put(parsed_args[2], BIOS_RED);
    //     put(": Write failed\n", BIOS_RED);
    //     return;
    // }

    updateDirectoryTable(current_directory);
}