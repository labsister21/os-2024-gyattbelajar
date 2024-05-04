#include "../lib-header/keyboard.h"
#include "../lib-header/framebuffer.h"
#include "../lib-header/portio.h"
#include "../lib-header/string.h"

// Global variable, defining position of row and column of the cursor
int current_framebuffer_pos_row = 0;
int current_framebuffer_pos_col = 0;


const char keyboard_scancode_1_to_ascii_map[256] = {
      0, 0x1B, '1', '2', '3', '4', '5', '6',  '7', '8', '9',  '0',  '-', '=', '\b', '\t',
    'q',  'w', 'e', 'r', 't', 'y', 'u', 'i',  'o', 'p', '[',  ']', '\n',   0,  'a',  's',
    'd',  'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',  'z', 'x',  'c',  'v',
    'b',  'n', 'm', ',', '.', '/',   0, '*',    0, ' ',   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0, '-',    0,    0,   0,  '+',    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,

      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
};


struct events {
    bool pressed[256];
} __attribute((packed));


static struct events pressed;
static struct KeyboardDriverState keyboard_state; 

// ASCII Map when shift pressed (urut, misal shift + 1 = !)
const char shift_map[] = {
    0, 0x1B, '!', '@', '#', '$', '%', '^',  '&', '*', '(',  ')',  '_', '+', '\b', '\t',
    'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',  'O', 'P', '{',  '}', '\n',   0,  'A',  'S',
    'D',  'F', 'G', 'H', 'J', 'K', 'l', ':', '\"', '~',   0, '|',  'Z', 'X',  'C',  'V',
    'B',  'N', 'M', '<', '>', '?',   0, '*',    0, ' ',   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0, '-',    0,    0,   0,  '+',    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,

      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
};

// Activate keyboard ISR / start listen keyboard & save to buffer
void keyboard_state_activate(void) {
        keyboard_state.keyboard_input_on = true;
}

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void) {
        keyboard_state.keyboard_input_on = false;
    
}

// Get keyboard buffer value and flush the buffer - @param buf Pointer to char buffer
void get_keyboard_buffer(char *buf) {
    memcpy(buf, keyboard_state.keyboard_buffer, KEYBOARD_BUFFER_SIZE);
}

bool shift_pressed() {
    return keyboard_state.shift_left || keyboard_state.shift_right;
}


void keyboard_isr(void) {
    uint8_t scancode = in(KEYBOARD_DATA_PORT);
    pressed.pressed[scancode] = true;

    if (!keyboard_state.keyboard_input_on) {
        keyboard_state.current_index = 0;
        return;
    }
    else {
        switch (scancode) {
            // capslock
            case 0x3a:
                keyboard_state.capslock = !keyboard_state.capslock; // check apakah capslock kondisi always on atau off
                break;
            // press shift left
            case 0x2a:
                keyboard_state.shift_left = true;
                break;
            // press right shift
            case 0x36:
                keyboard_state.shift_right = true;
                break;
            // release shift left
            case 0xaa:
                keyboard_state.shift_left = false;
                break;
            // release shift right
            case 0xb6:
                keyboard_state.shift_right = false;
                break;
            // press ctrl
            case 0x1d:
                keyboard_state.ctrl = true;
                break;
            // release ctrl
            case 0x9d:
                keyboard_state.ctrl = false;
                break;
            default:
                break;
            // alt, fx
        }
        char converted = keyboard_scancode_1_to_ascii_map[scancode];

        if (converted == 0) {
            pic_ack(IRQ_KEYBOARD);
            return;
        }
        // backspace
        else {
            if (converted == '\b') {
                // Cek apakah ada karakter lain pada keyboard_buffer
                if (keyboard_state.current_index > 0) {
                    // Sudah di ujung paling kiri
                    if (current_framebuffer_pos_col == 0) {
                        keyboard_state.current_index--;
                        keyboard_state.keyboard_buffer[keyboard_state.current_index] = ' ';
                        current_framebuffer_pos_col = COLUMN - 1;
                        current_framebuffer_pos_row--;
                        framebuffer_write(current_framebuffer_pos_row, current_framebuffer_pos_col, ' ', 0xFF, 0);
                        pic_ack(IRQ_KEYBOARD);
                        return;
                    }
                    // normal backspace
                    else {
                        keyboard_state.current_index--;
                        current_framebuffer_pos_col--;
                        keyboard_state.keyboard_buffer[keyboard_state.current_index] = ' '; // Hilangkan karakter jadi kosong (spasi)
                        framebuffer_write(current_framebuffer_pos_row, current_framebuffer_pos_col, ' ', 0xFF, 0);
                    }
                }
                // Character sudah habis
                else {
                    current_framebuffer_pos_col = 0;
                }
                framebuffer_set_cursor(current_framebuffer_pos_row, current_framebuffer_pos_col);
                pic_ack(IRQ_KEYBOARD);
                return;
            }
            // new line
            else if (converted == '\n') {
                memset(keyboard_state.keyboard_buffer, '\0', sizeof(keyboard_state.keyboard_buffer));
                keyboard_state.current_index = 0;
                keyboard_state.keyboard_input_on = 1;
                current_framebuffer_pos_row++;
                current_framebuffer_pos_col = 0;
                framebuffer_write(current_framebuffer_pos_row, current_framebuffer_pos_col, ' ', 0xFF, 0);
                framebuffer_set_cursor(current_framebuffer_pos_row, current_framebuffer_pos_col);
                pic_ack(IRQ_KEYBOARD);
                return;
            }
            else {
                if (converted >= 'a' && converted <= 'z') {
                    if (keyboard_state.capslock ^ shift_pressed()) {
                        converted = 'A' + converted - 'a';
                    } 
                }
                else if (shift_pressed() && converted < 97 && shift_map[(uint8_t)converted] != 0)
                {
                    converted = shift_map[(uint8_t)converted];
                }
                keyboard_state.keyboard_buffer[keyboard_state.current_index] = converted;
                keyboard_state.current_index++;
            }

            if (current_framebuffer_pos_col >= COLUMN) {
                current_framebuffer_pos_row++;
                current_framebuffer_pos_col = 0;
            }
            framebuffer_write(current_framebuffer_pos_row, current_framebuffer_pos_col, converted, 0xFF, 0);
            current_framebuffer_pos_col++;
        }
    framebuffer_set_cursor(current_framebuffer_pos_row, current_framebuffer_pos_col);}
    pic_ack(IRQ_KEYBOARD);
}
