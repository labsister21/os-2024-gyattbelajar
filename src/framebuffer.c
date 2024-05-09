#include "lib-header/framebuffer.h"
#include "lib-header/portio.h"
#include "lib-header/string.h"

struct Framebuffer framebuffer_state = {
    .col = 0,
    .row = 0
};

void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    uint16_t pos = r * COLUMN + c;
 
	out(0x3D4, 0x0F);
	out(0x3D5, (uint8_t) (pos & 0xFF));
	out(0x3D4, 0x0E);
	out(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    uint8_t *position = FRAMEBUFFER_MEMORY_OFFSET + COLUMN * 2 * row + col * 2;
    uint16_t attributes = (bg << 4) | (fg & 0x0F);

    memset(position, c, 1);
    memset(position + 1, attributes, 1);
}

void framebuffer_clear(void) {
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COLUMN; j++)
        {
            framebuffer_write(i, j, ' ', 0xFF, 0);
        }
    }
}

void putchar(char c, uint32_t color) {
    if (c != '\n') {
        framebuffer_write(framebuffer_state.row, framebuffer_state.col, c, color, 0);
    }
    if (framebuffer_state.col == COLUMN - 1 || c == '\n') {
        framebuffer_state.row++;
        framebuffer_state.col = 0;
        if (framebuffer_state.row == ROW) {
            scrollDown();
        }
    } else {
        framebuffer_state.col++;
    }
}

void clear_screen() {
    framebuffer_clear();
    framebuffer_state.row = 0;
    framebuffer_state.col = 0;
    framebuffer_set_cursor(0, 0);
}

void puts(const char* string, uint32_t count, uint32_t color) {
    uint32_t i;
    for (i = 0; i < count; i++) {
        putchar(string[i], color);
    }
}

void scrollDown() {
    memcpy(FRAMEBUFFER_MEMORY_OFFSET, FRAMEBUFFER_MEMORY_OFFSET + COLUMN * 2, COLUMN * 2 * (ROW - 1));
    framebuffer_state.row--;
    framebuffer_state.col = 0;
    for (int i = 0; i < COLUMN; i++) {
        framebuffer_write(framebuffer_state.row, i, ' ', 0xF, 0);
    }
}
