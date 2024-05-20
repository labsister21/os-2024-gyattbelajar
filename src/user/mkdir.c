#include "mkdir.h"
#include <stdint.h>
#include "command.h"

// argument is folder name
void mkdir (char argument[]) {       // Asumsi panjang len harus <= 8
    char curr_path[256];
    memcpy(curr_path, current_path, 256);
    strlen(curr_path);
    int32_t errorCode = 2;
    uint8_t name_len = strlen(argument);
    global_request.buffer_size = 0;
    global_request.buf = buf;
    while (name_len < 8) {      // Fill the rest with null
        argument[name_len] = '\0';
        name_len++;
    }
    memcpy(global_request.ext, "dir", 3); // Folder extension (dir
    global_request.parent_cluster_number = current_directory;  // Parent cluster number
    memcpy(global_request.name, argument, name_len);   // Folder name
    syscall(1, (uint32_t) &global_request, (uint32_t) &errorCode, 0); // syscall to create folder

    // Buat testing debug
    // struct FAT32DirectoryTable temp_dir_tab = dir_table;

    if (errorCode == 0) {     // Folder already exists
        put("Folder already exists!\n", BIOS_RED);
    } else if (errorCode == 2) {  // Folder not found
        memset(global_request.buf, 0, CLUSTER_SIZE);
        syscall(2, (uint32_t) &global_request, (uint32_t) &errorCode, 0); // syscall to write folder
        memcpy(current_path, curr_path, 256);
        if (errorCode == 0) {
            put("Folder is successfully created!\n", BIOS_LIGHT_GREEN);
            updateDirectoryTable(current_directory);
            int cluster_now = findEntryCluster(global_request.name);
            if (cluster_now) {
                int root = current_directory;
                updateDirectoryTable(current_directory);
                struct FAT32DirectoryEntry tempRoot = dir_table.table[0];
                memcpy(&tempRoot.name, &dir_table.table[0].name, 8);
                memcpy(&tempRoot.ext, &dir_table.table[0].ext, 3);
                tempRoot.attribute = dir_table.table[0].attribute;
                tempRoot.user_attribute = dir_table.table[0].user_attribute;
                tempRoot.undelete = dir_table.table[0].undelete;
                tempRoot.create_date = dir_table.table[0].create_date;
                tempRoot.create_time = dir_table.table[0].create_time;
                tempRoot.cluster_high = dir_table.table[0].cluster_high;
                tempRoot.cluster_low = dir_table.table[0].cluster_low;
                updateDirectoryTable(cluster_now);
                memcpy(dir_table.table[1].name, tempRoot.name, 8);
                memcpy(dir_table.table[1].ext, tempRoot.ext, 3);
                dir_table.table[1].attribute = tempRoot.attribute;
                dir_table.table[1].user_attribute = tempRoot.user_attribute;
                dir_table.table[1].undelete = tempRoot.undelete;
                dir_table.table[1].create_date = tempRoot.create_date;
                dir_table.table[1].create_time = tempRoot.create_time;
                dir_table.table[1].cluster_high = tempRoot.cluster_high;
                dir_table.table[1].cluster_low = tempRoot.cluster_low;
                syscall(10, (uint32_t) &dir_table.table, cluster_now, 1);
                updateDirectoryTable(root);
            }
        }
        else {
            put("Failed to create folder after trying to write folder! Unknown error\n", BIOS_RED);
        }
    }
    else {
        put("Failed to create folder! Unknown error\n", BIOS_RED);
    }
}