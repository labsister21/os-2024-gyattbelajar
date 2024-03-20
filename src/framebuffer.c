#include "lib-header/framebuffer.h"
#include "lib-header/portio.h"
#include "lib-header/string.h"

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