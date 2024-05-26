#include "cd.h"

void cd(char argument[]) {
    uint32_t search_dir_num = current_directory;
    char* name = "\0\0\0\0\0\0\0\0";
    updateDirectoryTable(search_dir_num);

    if (strlen(argument) > 8) {
        put("Directory name is too long!\n", BIOS_RED);
        return;
    }
    else if (strcmp(argument, "..", strlen(argument)) == 0) {
        if (current_directory == ROOT_CLUSTER_NUMBER) {
            put("Already in root directory!\n", BIOS_RED);
            return;
        }
        else {
            // TODO : Move to parent directory
            current_directory = dir_table.table[1].cluster_low;

            // Remove the last directory from the current path
            int path_len = strlen(current_path);
            current_path[path_len-1] = '\0';
            int i = path_len - 2;
            while (current_path[i] != '/' && i >= 0) {
                current_path[i] = '\0';
                i--;
            }
            updateDirectoryTable(current_directory);
        }
    }
    else {
        memcpy(name, argument, strlen(argument));
        int entry_idx = findEntryIndex(name);
        if (entry_idx == -1) {
            put("Directory not found!\n", BIOS_RED);
            return;
        }
        else {
            if (dir_table.table[entry_idx].attribute == 1) {
                current_directory = findEntryCluster(name);
                
                // Add name to the current path manually
                int path_len = strlen(current_path);
                for (int i = 0; i < (int) strlen(argument); i++) {
                    if (current_path[path_len] == '\0') {
                        current_path[path_len] = argument[i];
                        path_len++;
                    }
                }
                current_path[path_len] = '/';
            }
            else {
                put("Folder name is not a directory\n", BIOS_RED);
            }
        }
    }
}
