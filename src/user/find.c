#include "find.h"
#include "command.h"
#include "../file-system/fat32.h"

// find command
void find(char* filename) {
    updateDirectoryTable(current_directory);

    char name[8] = "\0\0\0\0\0\0\0\0";
    int len = strlen(filename);
    int i = 0;
    while (i < len && filename[i] != '.' && i < 8) {
        name[i] = filename[i];
        i++;
    }
    i += 1;
    int k = 0;
    char ext[3] = "\0\0\0";

    while (i < len && k < 3) {
        ext[k] = filename[i];
        k++;
        i++;
    }

    bool found = false;
    for (i = 2; i <  64; i++) {
        if (dir_table.table[i].user_attribute == UATTR_NOT_EMPTY && strcmp(dir_table.table[i].name, name, 8) == 0) {
            // if file check the extention
            if(strcmp(dir_table.table[i].ext, ext, 3) == 0 && dir_table.table[i].attribute == 0){
                put(dir_table.table[i].name, BIOS_LIGHT_BLUE);
                put(".", BIOS_LIGHT_BLUE);
                put(dir_table.table[i].ext, BIOS_LIGHT_BLUE);
                put("\n", BIOS_BLACK);
                found = true;
            } else {
                put(dir_table.table[i].name, BIOS_LIGHT_BLUE);
                put("\n", BIOS_BLACK);
                found = true;
            }
            continue;
        }

        // check files inside directories
        if(dir_table.table[i].user_attribute == UATTR_NOT_EMPTY && dir_table.table[i].attribute == 1){
            findRecursive(name, ext, dir_table.table[i].cluster_low, &found);
        }
    }

    if(!found){
        put("find: '", BIOS_RED);
        put(filename, BIOS_RED);
        put("': No such file or directory\n", BIOS_RED);
    }

    updateDirectoryTable(current_directory);
}

void findRecursive(char *name, char* ext, int cluster_number, bool *found) {
    updateDirectoryTable(cluster_number);
    for (int i = 2; i <  64; i++) {
        if (dir_table.table[i].user_attribute == UATTR_NOT_EMPTY && strcmp(dir_table.table[i].name, name, 8) == 0) {
            // if file check the extention
            if(strcmp(dir_table.table[i].ext, ext, 3) == 0 && dir_table.table[i].attribute == 0){
                put(name, BIOS_LIGHT_BLUE);
                put(".", BIOS_LIGHT_BLUE);
                put(dir_table.table[i].ext, BIOS_LIGHT_BLUE);
                put("\n", BIOS_BLACK);
                *found = true;
            } else {
                put(name, BIOS_LIGHT_BLUE);
                put("\n", BIOS_BLACK);
                *found = true;
            }
            continue;
        }
        
        // check files inside directories
        if(dir_table.table[i].user_attribute == UATTR_NOT_EMPTY && dir_table.table[i].attribute == 1){
            findRecursive(name, ext, dir_table.table[i].cluster_low, found);
        }
    }

    updateDirectoryTable(dir_table.table[1].cluster_low);
}