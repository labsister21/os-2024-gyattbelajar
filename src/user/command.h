#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <stdint.h>
#include "../lib-header/string.h"
#include "../file-system/fat32.h"
#include "syscall.h"
#include "find.h"
#include "cp.h"
#include "rm.h"
#include "mv.h"
#include "mkdir.h"
#include "cd.h"
#include "cat.h"
#include "ls.h"
#include "touch.h"

// definisi warna
#define BIOS_LIGHT_GREEN 0b1010
#define BIOS_GREY        0b0111
#define BIOS_DARK_GREY   0b1000
#define BIOS_LIGHT_BLUE  0b1001
#define BIOS_RED         0b1100
#define BIOS_BROWN       0b0110
#define BIOS_WHITE       0b1111
#define BIOS_BLACK       0b0000
void put_template();

struct CursorPosition {
    int row; // current row cursor
    int col; // current col cursor
    char buffer[2048]; // char buffer, input
    int current_length; // current length of text, initially 0
    char current_char;
    int start_col;
};

extern struct CursorPosition CP;

// current directory cluster number
extern uint32_t current_directory;

// current directory table
extern struct FAT32DirectoryTable dir_table;

// dipake di mkdir
extern struct ClusterBuffer cl[2];
extern struct FAT32DriverRequest global_request;

// to store user input?
extern char buf[256];

// to store the current path
extern char current_path[128];

// initialize current path with \0
void init_current_path();

// show template with path, pastikan path depannya ada /{path}
void put_template_with_path(char* path);

// Wrapper for the base interrupt
void put(char* buf, uint8_t color);

// Print starting screen text
void print_starting_screen();

// parse input, into each argument
// Return amount of argument
int inputparse (char *args_val, char parsed_args[5][128]);

// shell command 
void start_command();

// Check if the path is absolute
bool is_path_absolute(char (*path));

// parse arguments
int parse_path(char *path, char directories[12][13]);

// Update the dir_table according to the cluster number
void updateDirectoryTable(uint32_t cluster_number);

// Find the name of directory in the dir_table and return its cluster number
int findEntryCluster(char* name);

int findEntryIndex(char* name);
#endif