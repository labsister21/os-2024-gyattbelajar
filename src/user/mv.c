#include "mv.h"
#include "ls.h"
#include "command.h"
#include "../file-system/fat32.h"

// move and rename file
// parsed_args[1] -> source
// parsed_args[2] -> destination
void mv(char (*parsed_args)[128]){
    updateDirectoryTable(current_directory);

    // TODO handle in folder
    // Source file parse
    char srcname[8];
    memset(srcname, '\0', 8);
    int len = strlen(parsed_args[1]);
    int i = 0;
    while (i < len && parsed_args[1][i] != '.' && i < 8) {
        srcname[i] = parsed_args[1][i];
        i++;
    }
    i += 1;
    int k = 0;
    char srcext[3]; 
    memset(srcext, '\0', 3);

    while (i < len && k < 3) {
        srcext[k] = parsed_args[1][i];
        k++;
        i++;
    }

    struct FAT32DirectoryTable tempDir = dir_table;
    tempDir = dir_table;

    // check for src file
    bool same = false;
    for (i = 0; i < 63; i++) {
        if (strcmp(srcname, tempDir.table[i].name, 8) == 0 && strcmp(tempDir.table[i].ext, srcext, 3) == 0) {
            same = true;
            break;
        }
    } 

    if (!same) {
        put("File src not found!\n", BIOS_RED);
    }

    // Destination file parse
    char destname[8];
    memset(destname, '\0', 8);
    len = strlen(parsed_args[1]);
    i = 0;
    while (i < len && parsed_args[2][i] != '.' && i < 8) {
        destname[i] = parsed_args[2][i];
        i++;
    }
    i += 1;
    k = 0;
    char destext[3];
    memset(destext, '\0', 3);

    while (i < len && k < 3) {
        destext[k] = parsed_args[2][i];
        k++;
        i++;
    }

    // TODO handle in folder

    // Read file src
    struct FAT32DriverRequest src = {
        .buffer_size = CLUSTER_SIZE,
        .parent_cluster_number = current_directory,
    };
    memset(&src.name, '\0', 8);
    memset(&src.ext, '\0', 3);
    memcpy(&src.name, srcname, 8);
    memcpy(&src.ext, srcext, 3);

    int retcode;
    syscall(0, (uint32_t) &src, (uint32_t) &retcode, 0);
    if (retcode == 3) {
        put("Read failed", BIOS_RED);
        put("\n", BIOS_RED);
        return;
    }

    // Write into new file
    src.parent_cluster_number = current_directory; // Change to dest folder value
    memset(&src.name, '\0', 8);
    memset(&src.ext, '\0', 3);
    memcpy(&src.name, destname, 8);
    memcpy(&src.ext, destext, 3);
    syscall(2, (uint32_t) &src, (uint32_t) &retcode, 0);

    updateDirectoryTable(current_directory);

    // Delete the src file
    memset(&src.name, '\0', 8);
    memset(&src.ext, '\0', 3);
    memcpy(&src.name, srcname, 8);
    memcpy(&src.ext, srcext, 3);
    src.parent_cluster_number = current_directory;
    syscall(3, (uint32_t) &src, (uint32_t) &retcode, 0);

    if (retcode != 0) {
        put("Failed to write\n", BIOS_RED);
    } else {
        put("Successfully move file!\n", BIOS_LIGHT_BLUE);
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