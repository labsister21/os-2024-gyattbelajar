#ifndef __FIND_H__
#define __FIND_H__

#include <stdint.h>
#include <stdbool.h>

// find command
void find(char* filename);

void findRecursive(char *name, char* ext, int cluster_number, bool *found);

#endif