#ifndef DOT_MATRIX_FONT_H
#define DOT_MATRIX_FONT_H

enum {
    FONT_COUNT = 10,
    ROW_COUNT = 10
};

unsigned char g_dot_matrix_font[FONT_COUNT][ROW_COUNT] = {
    {0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
    {0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
    {0x7e,0x7e,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
    {0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
    {0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
    {0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
    {0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
    {0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
    {0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
    {0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03}  // 9
};

unsigned char g_dot_matrix_full[ROW_COUNT] = {
    0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
};

unsigned char g_dot_matrix_black[ROW_COUNT] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

#endif