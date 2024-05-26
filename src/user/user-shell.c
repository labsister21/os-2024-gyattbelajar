#include <stdint.h>
#include "../file-system/fat32.h"
#include "command.h"
#include "../lib-header/string.h"


int main(void) {
    print_starting_screen();

    // Activate keyboard
    syscall(7, 0, 0, 0);

    // listeners
    start_command();
    
    return 0;
}