#include "mkdir.h"
#include "stdint.h"
#include "command.h"

// argument is folder name
void mkdir (char argument[]) {       // Asumsi panjang len harus <= 8
    int32_t retcode;
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
    syscall(1, (uint32_t) &global_request, (uint32_t) &retcode, 0); // syscall to create folder
    if (retcode == 0) {     // Folder already exists
        put("Folder already exists!\n", BIOS_RED);
    } else if (retcode == 2) {  // Folder not found
        memset(global_request.buf, 0, CLUSTER_SIZE);
        syscall(2, (uint32_t) &global_request, (uint32_t) &retcode, 0); // syscall to write folder
        if (retcode == 0) {
            put("Folder is successfully created!\n", BIOS_LIGHT_GREEN);
        }
        else {
            put("Failed to create folder! Unknown error\n", BIOS_RED);
        }
    }
    else {
        put("Failed to create folder! Unknown error\n", BIOS_RED);
    }
}