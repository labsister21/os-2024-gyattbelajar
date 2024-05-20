#include "touch.h"
#include "command.h"
#include "../file-system/fat32.h"

/**
 * Membuat file di current directory
 * Otomatis memisahkan extension selama ada titik
*/
void makefile(char *filename) {
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

    // Create file FAT32DriverRequest
    struct FAT32DriverRequest request = {
        .buf                   = (uint8_t*) 0,
        .parent_cluster_number = current_directory,
        .buffer_size           = CLUSTER_SIZE,
    };

    // Fill the name and extension
    memcpy(&request.name, name, 8);
    memcpy(&request.ext, ext, 3);
    
    // Write request
    syscall(2, (uint32_t) &request, 0, 0);

}