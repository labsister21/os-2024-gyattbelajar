#ifndef __FIND_H__
#define __FIND_H__

#include <stdint.h>
#include <stdbool.h>

// find command
void find(char (*parsed_args)[128]);

void find_recursive(char* dir_name, char* parent_path, uint32_t parent_cluster, char directories[12][13], int curr_directory_index, int dir_count, bool *haveFound);

void find2(char* filename);

void print_absolute_path(int current_file);

#endif