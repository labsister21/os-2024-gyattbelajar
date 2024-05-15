#include "mv.h"
#include "command.h"
#include "../file-system/fat32.h"

void mv(char (*parsed_args)[128]){
    char src_directories[12][13];
    int src_dir_count = parse_path(parsed_args[1], src_directories);
    if(src_dir_count == -1){
        put("cp: ", BIOS_RED);
        put(parsed_args[1], BIOS_RED);
        put(": source path not valid\n", BIOS_RED);
        return;
    }
}