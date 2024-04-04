#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../lib-header/string.h"
#include "fat32.h"

const Directory_Table_Size = (CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry));

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

struct FAT32DriverState fileSystem = {};

uint32_t cluster_to_lba(uint32_t cluster){
    return cluster * CLUSTER_SIZE;
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count){
    write_blocks(ptr, cluster_to_lba(cluster_number), cluster_count * CLUSTER_BLOCK_COUNT);
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count){
    read_blocks(ptr, cluster_to_lba(cluster_number), cluster_count * CLUSTER_BLOCK_COUNT);
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster){
    struct FAT32DirectoryEntry* t = dir_table->table;
    memcpy(t[0].name, name, 8);
    t[0].user_attribute = UATTR_NOT_EMPTY;
    t[0].undelete = false;
    t[0].cluster_high = (parent_dir_cluster >> 16) & 0xffff;
    t[0].cluster_low = parent_dir_cluster & 0xffff;
    for (int i=1; i < Directory_Table_Size; i++){
        t[i].user_attribute = !UATTR_NOT_EMPTY;
    }
    fileSystem.fat_table.cluster_map[parent_dir_cluster] = FAT32_FAT_END_OF_FILE;
    write_clusters(&fileSystem.fat_table, 1, 1);
    write_clusters(dir_table->table, parent_dir_cluster, 1);
}

bool is_empty_storage(void){
    struct BlockBuffer b;

    // Read the boot sector block into b
    read_blocks(&b.buf, BOOT_SECTOR, 1);

    // Return if b is not the same as fs_signature
    return memcmp(fs_signature, b.buf, BLOCK_SIZE) != 0;
}

void initialize_filesystem_fat32(void){
    if (is_empty_storage()){
        create_fat32();
    } else {
        read_clusters(&fileSystem.fat_table, FAT_CLUSTER_NUMBER, 1);
    }
}

void create_fat32(void){
    fileSystem.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fileSystem.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    fileSystem.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    int index;
    for (index = 3; index < CLUSTER_MAP_SIZE; index++){
        fileSystem.fat_table.cluster_map[index] = 0;
    }
    // Writing signature into Disk
    write_blocks(fs_signature, BOOT_SECTOR, 1);

    // Writing FAT Table cluster 1 into Disk
    write_clusters(&fileSystem.fat_table, FAT_CLUSTER_NUMBER, 1);

    // Initialize Root Directory
    init_directory_table(&fileSystem.dir_table_buf, "root", 2);
}

int8_t read_directory(struct FAT32DriverRequest request){

    read_clusters(&fileSystem.dir_table_buf, request.parent_cluster_number, 1);

    struct FAT32DirectoryEntry* entries = fileSystem.dir_table_buf.table;

    bool folderFound = false;

    for (uint8_t i=0; i < Directory_Table_Size; i++){

        // Check if there is an entry with the same name
        if (!strcmp(entries[i].name, request.name, 8)){
            folderFound = true;

            // Check if the found entry is a folder
            if (entries[i].attribute == 1){

                // Folder is found
                read_clusters(request.buf, entries[i].cluster_low ,1);
                return 0;
            }
            return 1;
        }
    }

    // folder with the requested name is not found
    if(!folderFound){
        return 2;
    }
}

int8_t read(struct FAT32DriverRequest request){
    
    read_clusters(&fileSystem.dir_table_buf, request.parent_cluster_number, 1);

    struct FAT32DirectoryEntry* entries = fileSystem.dir_table_buf.table;

    bool fileFound = false;

    for (uint8_t i=0; i < Directory_Table_Size; i++){

        // Check if there is an entry with the same name
        if (!strcmp(entries[i].name, request.name, 8) && !strcmp(entries[i].ext, request.ext, 3)){
            fileFound = true;

            // Check if the found entry is a folder
            if (entries[i].attribute == 1){
                return 1;
            }

            // File is found, check if buffer is not enough
            if (request.buffer_size < entries[i].filesize){
                return 2;
            }

            // Request buffer can be loaded
            uint16_t numCluster = entries[i].filesize / CLUSTER_SIZE + 1;
            uint16_t clusterRead = entries[i].cluster_low;
            for (uint16_t index=0; index<numCluster; index++){
                read_clusters(index * CLUSTER_SIZE + request.buf, clusterRead, 1);
                clusterRead = fileSystem.fat_table.cluster_map[clusterRead];
            }
            return 0;
        }
    }

    // file with the requested name is not found
    if(!fileFound){
        return 3;
    }
}

int8_t write(struct FAT32DriverRequest request){

    // if parent cluster isn't valid
    if (fileSystem.fat_table.cluster_map[request.parent_cluster_number] != FAT32_FAT_END_OF_FILE)
    {
        return 2;
    }

    // read the parent cluster into dir_table_buf
    read_clusters(&fileSystem.dir_table_buf, request.parent_cluster_number, 1);

    struct FAT32DirectoryEntry *entries = fileSystem.dir_table_buf.table;
    uint16_t numCluster = request.buffer_size / CLUSTER_SIZE + 1;

    numCluster = max(numCluster, 1);

    uint16_t numClusterFound = 0;
    uint16_t loc[numCluster];
    uint16_t directoryLoc = -1;
    uint16_t currCluster = 3;

    for (uint8_t i=1; i<Directory_Table_Size; i++){
        if (entries[i].user_attribute != UATTR_NOT_EMPTY){

            // Searching empty cluster at FAT32
            while (numClusterFound < numCluster && currCluster < CLUSTER_MAP_SIZE){
                if (fileSystem.fat_table.cluster_map[currCluster] == 0){  
                    loc[numClusterFound] = currCluster;
                    numClusterFound++;
                }
                currCluster++;
            }

            directoryLoc = i;
            break;
        }

        // Check whether an entry with the same name exits
        if (!strcmp(entries[i].name, request.name, 8)){

            // Check whether the entry is a folder
            if (entries[i].attribute == 1 && request.buffer_size == 0){
                return 1;
            }

            // Check whether the entry is a file
            if (!strcmp(entries[i].ext, request.ext, 3) && request.buffer_size != 0){
                return 1;
            }
        }
    }

    // Number or clusters available is not enough to write, unknown error
    if (numClusterFound < numCluster){
        return -1;
    }

    // Initialize the new entry
    memcpy(entries[directoryLoc].name, request.name, 8);
    entries[directoryLoc].user_attribute = UATTR_NOT_EMPTY;
    entries[directoryLoc].cluster_high = (loc[0] >> 16) & 0xFFFF;
    entries[directoryLoc].cluster_low = loc[0] & 0xFFFF;

    // Identify the type of the requested new entry
    if (request.buffer_size == 0){

        // If it's a new folder
        entries[directoryLoc].attribute = 1;
        
        // Initialize the new folder directory table
        init_directory_table(request.buf, request.name, loc[0]);
    } else {

        // If it's a new file
        entries[directoryLoc].attribute = 0;
        memcpy(entries[directoryLoc].ext, request.ext, 3);
        entries[directoryLoc].filesize = request.buffer_size;

        // Write the file 
        for (uint16_t i = 0; i < numCluster; i++){
            write_clusters(request.buf + CLUSTER_SIZE * i, loc[i], 1);

            // On the last cluster to write
            if (i == numCluster - 1){
                fileSystem.fat_table.cluster_map[loc[i]] = FAT32_FAT_END_OF_FILE;
            } else {
                fileSystem.fat_table.cluster_map[loc[i]] = loc[i+1];
            }
        }

        // Save the new FAT32 table to disk
        write_clusters(&fileSystem.fat_table, 1, 1);
    }

    // Save the new parent directory table
    write_clusters(entries, request.parent_cluster_number, 1);

    // Writing success
    return 0;

}

int8_t delete(struct FAT32DriverRequest request){

    // Set-up entries as table of requested directory
    read_clusters(&fileSystem.dir_table_buf, request.parent_cluster_number, 1);
    struct FAT32DirectoryEntry* entries = fileSystem.dir_table_buf.table;

    for (uint8_t i=0; i < Directory_Table_Size; i++){

        // Skip empty clusters
        if (entries[i].user_attribute != UATTR_NOT_EMPTY){
            continue;
        }

        // Check if name is as requested
        if (strcmp(request.name, entries[i].name, 8)){

            // Check the type of entry based on attribute
            if (entries[i].attribute){

                // If entry is a directory
                struct FAT32DirectoryTable dt;
                read_clusters(&dt, entries[i].cluster_low, 1);
                for (unsigned int j=1; j < Directory_Table_Size; j++){
                    if (dt.table[j].user_attribute == UATTR_NOT_EMPTY){

                        // Directory is not empty, return non empty error
                        return 2;
                    }
                }

                // Directory guaranteed to be empty, delete it
                fileSystem.fat_table.cluster_map[entries[i].cluster_low] = 0;
                entries[i].user_attribute = !UATTR_NOT_EMPTY;
                entries[i].undelete = true;

                // Save into parent directory table
                write_clusters(&fileSystem.dir_table_buf, request.parent_cluster_number, 1);

                // Save FAT32 into Disk
                write_clusters(&fileSystem.fat_table, 1, 1);

                // Deletion Sucessful
                return 0;
            } else {

                // If entry is a file, make sure whether the extension is same as requested
                if (!strcmp(request.ext, entries[i].ext, 3)){

                    // Similiar Extension, delete file
                    entries[i].user_attribute = !UATTR_NOT_EMPTY;
                    entries[i].undelete = true;

                    // Save into parent directory table
                    write_clusters(&fileSystem.dir_table_buf, request.parent_cluster_number, 1);

                    // Prepare for deleting file content
                    uint16_t numCluster = entries[i].filesize / CLUSTER_SIZE + 1;
                    uint16_t toZeros[numCluster];
                    uint16_t currCluster = entries[i].cluster_low;

                    // List all clusters to be zeroed
                    for (uint16_t j=0; j<numCluster; j++){
                        toZeros[i] = currCluster;
                        currCluster = fileSystem.fat_table.cluster_map[currCluster];
                    }

                    // Zeroing all clusters from the list
                    for (uint16_t j=0; j<numCluster; j++){
                        fileSystem.fat_table.cluster_map[toZeros[i]] = 0;
                    }

                    // Save FAT32 into disk
                    write_clusters(&fileSystem.fat_table, 1, 1);

                    // Deletion Sucessful
                    return 0;
                } else {

                    // Different extension, do nothing
                    continue;
                }
            }
        }
    }

    // No file with the requested name, return not found error
    return 1;
}