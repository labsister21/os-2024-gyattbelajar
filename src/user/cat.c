#include "cat.h"

void cat(char argument[]) {
    put(argument, BIOS_BLACK);
    put("cat command\n", BIOS_LIGHT_GREEN);
}