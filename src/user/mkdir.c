#include "mkdir.h"
#include "stdint.h"
#include "command.h"

// argument is folder name
void mkdir (char *argument) {       // Asumsi panjang len harus <= 8
    int32_t retcode;
    uint8_t name_len = strlen(argument);
    request.buffer_size = 0;
    request.buf = 0;
    while (name_len < 8) {      // Fill the rest with null
        argument[name_len] = '\0';
        name_len++;
    }
    memcpy(request.ext, "dir", 3); // Folder extension (dir
    request.parent_cluster_number = current_directory;  // Parent cluster number
    memcpy(request.name, argument, name_len);   // Folder name
    syscall(1, (uint32_t) &request, (uint32_t) &retcode, 0); // syscall to create folder
    if (retcode == 0) {     // Folder already exists
        put("Folder already exists!\n", BIOS_RED);
    } else if (retcode == 1) {  // Folder not found
        memset(request.buf, 0, request.buffer_size);
        syscall(2, (uint32_t) &request, (uint32_t) &retcode, 0); // syscall to write folder
        if (retcode == 0) {
            put("Folder is successfully created!\n", BIOS_LIGHT_GREEN);
        }
        else {
            put("Failed to create folder! Unknown error\n", BIOS_RED);
        }
    }
}

// void mkdir(char* req_buf, uint32_t cluster_number, char* argument1, int argument1_length, struct FAT32DriverRequest request){
//   uint32_t retcode;
//   request.buffer_size = 0;
//   request.buf = req_buf;
//   memcpy(request.name, argument1, argument1_length);
//   int idx = argument1_length;
//   while (idx <= 7) {
//     request.name[idx] = '\0';
//     idx++;
//   }
//   memcpy(request.ext, "dir", 3);
//   request.parent_cluster_number = cluster_number;
//   syscall(1, (uint32_t) &request, (uint32_t) &retcode, 0);
//   if (retcode == READ_DIRECTORY_REQUEST_SUCCESS) {
//     puts("Folder already exists!\n", BIOS_LIGHT_MAGENTA);
//   } else if (retcode == READ_DIRECTORY_REQUEST_FOLDER_NOT_FOUND) {
//     memset(req_buf, 0, BUFFER_SIZE);
//     syscall(2, (uint32_t) &request, (uint32_t) &retcode, 0);
//     puts("Folder is successfully created!\n", BIOS_LIGHT_MAGENTA);
//   }
// }

// void mkdir(char *path) {
//   char req_buf[BUFFER_SIZE];
//   struct FAT32DriverRequest request;
//   uint32_t retcode;
//   request.buffer_size = BUFFER_SIZE;
//   request.buf = req_buf;
//   int idx = 0;
//   int argument1_length = 0;
//   while (path[idx] != '\0' && path[idx] != ' ') {
//     idx++;
//     argument1_length++;
//   }
//   memcpy(request.name, path, argument1_length);
//   while (idx <= 7) {
//     request.name[idx] = '\0';
//     idx++;
//   }
//   memcpy(request.ext, "dir", 3);
//   request.parent_cluster_number = current_directory_cluster_number;
//   syscall(1, (uint32_t) &request, (uint32_t) &retcode, 0);
//   if (retcode == READ_DIRECTORY_REQUEST_SUCCESS) {
//     puts("Folder already exists!\n", BIOS_LIGHT_MAGENTA);
//   } else if (retcode == READ_DIRECTORY_REQUEST_FOLDER_NOT_FOUND) {
//     memset(req_buf, 0, BUFFER_SIZE);
//     syscall(2, (uint32_t) &request, (uint32_t) &retcode, 0);
//     puts("Folder is successfully created!\n", BIOS_LIGHT_MAGENTA);
//   }
// }