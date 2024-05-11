#include <stdint.h>

// definisi warna
#define BIOS_LIGHT_GREEN 0b1010
#define BIOS_GREY        0b0111
#define BIOS_DARK_GREY   0b1000
#define BIOS_LIGHT_BLUE  0b1001
#define BIOS_RED         0b1100
#define BIOS_BROWN       0b0110
#define BIOS_WHITE       0b1111
#define BIOS_BLACK       0b0000

// definisi struct posisi kursor dan character buffer
struct CursorPosition {
    int row; // current row cursor
    int col; // current col cursor
    char buffer[2048]; // char buffer, input
    int current_length; // current length of text, initially 0
    char current_char;
};

extern struct CursorPosition CP;

// syscall ke interrupt
void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

// show template tanpa path
void put_template();

// show template with path, pastikan path depannya ada /{path}
void put_template_with_path(char* path);

void get_command();