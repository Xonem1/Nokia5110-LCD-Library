#include "Nokia5110.h"

// the memory buffer for the LCD
uint8_t buffer[LCD_BYTES];

Nokia_5110::Nokia_5110(PinName sce, PinName rst, PinName dc, PinName dn,
                       PinName sclk) {
    _lcd_SPI = new SPI(dn, NC, sclk);
    _lcd_SPI->format(LCD_SPI_BITS, LCD_SPI_MODE);
    _lcd_SPI->frequency(LCD_SPI_FREQ);

    _sce = new DigitalOut(sce, 1);
    _rst = new DigitalOut(rst, 1);
    _dc = new DigitalOut(dc, 0);
}

void Nokia_5110::init(uint8_t con, uint8_t bias) {
    reset();
    wait_ms(10);
    set_contrast(con);
    set_bias(bias);
    set_mode(LCD_DISPLAYNORMAL);
}

void Nokia_5110::reset() {
    _rst->write(0);
    wait_ms(500);
    _rst->write(1);
}

void Nokia_5110::send_command(uint8_t cmd) {
    _sce->write(0);

    _lcd_SPI->write(cmd);

    _sce->write(1);
}

void Nokia_5110::send_data(uint8_t data) {
    _dc->write(1);
    _sce->write(0);

    _lcd_SPI->write(data);

    _sce->write(1);
    _dc->write(0);
}

void Nokia_5110::set_contrast(uint8_t con) {
    if (con > 0x7f)
        con = 0x7f;

    send_command(LCD_FUNCTIONSET | LCD_EXTENDEDINSTRUCTION);
    send_command(LCD_SETVOP | con);
    send_command(LCD_FUNCTIONSET);
}

void Nokia_5110::set_bias(uint8_t bias) {
    if (bias > 0x08)
        bias = 0x08;

    send_command(LCD_FUNCTIONSET | LCD_EXTENDEDINSTRUCTION);
    send_command(LCD_set_bias | bias);
    send_command(LCD_FUNCTIONSET);
}

void Nokia_5110::set_mode(uint8_t mode) {
    if (mode > 0x08)
        mode = 0x08;

    send_command(LCD_DISPLAYCONTROL | mode);
}

void Nokia_5110::set_power(uint8_t pow) {
    pow = pow ? 0 : LCD_POWERDOWN;
    send_command(LCD_FUNCTIONSET | pow);
}

void Nokia_5110::set_x_addr(uint8_t col) {
    col %= LCD_WIDTH;
    send_command(LCD_SETXADDR | col);
}

void Nokia_5110::set_y_addr(uint8_t bank) {
    bank %= LCD_BANKS;
    send_command(LCD_SETYADDR | bank);
}

void Nokia_5110::set_cursor(uint8_t col, uint8_t bank) {
    set_x_addr(col);
    set_y_addr(bank);
}

void Nokia_5110::clear_buffer() {
    for (unsigned int i = 0; i < LCD_BYTES; i++) {
        buffer[i] = 0x00;
    }
}

void Nokia_5110::display() {
    set_y_addr(0);
    set_x_addr(0);
    for (unsigned int i = 0; i < LCD_BYTES; i++) {
        send_data(buffer[i]);
    }
}

void Nokia_5110::draw_pixel(uint8_t col, uint8_t row, uint8_t value,
                            DrawMode drawMode) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;
    switch (drawMode) {
    case pixel_set:
        if (value)
            buffer[col + (row / 8) * LCD_WIDTH] |= (1 << (row % 8));
        else
            buffer[col + (row / 8) * LCD_WIDTH] &= ~(1 << (row % 8));
        break;
    case pixel_or:
        if (value)
            buffer[col + (row / 8) * LCD_WIDTH] |= (1 << (row % 8));
        break;
    case pixel_xor:
        if (value)
            buffer[col + (row / 8) * LCD_WIDTH] ^= (1 << (row % 8));
        break;
    case pixel_clear:
        if (value)
            buffer[col + (row / 8) * LCD_WIDTH] &= ~(1 << (row % 8));
        break;
    }
}

uint8_t Nokia_5110::get_pixel(uint8_t col, uint8_t row) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;

    return buffer[col + (row / 8) * LCD_WIDTH] & (1 << (row % 8));
}

void Nokia_5110::draw_byte(uint8_t col, uint8_t bank, uint8_t byte) {
    col %= LCD_WIDTH;
    bank %= LCD_BANKS;

    buffer[col + bank * LCD_WIDTH] = byte;
}

uint8_t Nokia_5110::get_byte(uint8_t col, uint8_t bank) {
    col %= LCD_WIDTH;
    bank %= LCD_BANKS;

    return buffer[col + bank * LCD_WIDTH];
}

void Nokia_5110::print_char(char c, uint8_t col, uint8_t row, DrawMode mode) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;

    c -= 32;

    for (unsigned int i = 0; i < 5; i++) {
        for (unsigned int b = 0; b < 8; b++) {
            draw_pixel(col + i, row + b, font[(5 * c) + i] & (1 << b), mode);
        }
    }
}

void Nokia_5110::print_string(const char *str, uint8_t col, uint8_t row,
                              DrawMode mode) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;

    while (*str && col + 6 < LCD_WIDTH) {
        print_char(*str, col, row, mode);
        col += 6;
        str++;
    }
}

void Nokia_5110::draw_bitmap(const uint8_t *bmp, uint8_t col, uint8_t row,
                             uint8_t width, uint8_t height, DrawMode mode) {
    uint8_t mask = 0x80;

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            draw_pixel(col + x, row + y, *bmp & mask, mode);
            mask >>= 1;

            if (mask == 0) { // if we reached the end of the byte
                mask = 0x80;
                bmp++;
            }
        }
    }
}

void Nokia_5110::draw_wbitmap(const uint8_t *wbmp, uint8_t col, uint8_t row,
                              DrawMode mode) {
    if (*wbmp++ != 0x00) // image type, only supports 0
        return;
    if (*wbmp++ != 0x00) // always 0
        return;
    uint8_t width = *wbmp++;  // image width in pixels
    uint8_t height = *wbmp++; // image height in pixels

    uint8_t mask = 0x80;

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            draw_pixel(col + x, row + y, *wbmp & mask, mode);
            mask >>= 1;

            if (mask == 0) { // if we reached the end of the byte
                mask = 0x80;
                wbmp++;
            }
        }
        if (mask != 0x80) {
            mask = 0x80; // wbmps pad out the end of each row, so reset the mask
            wbmp++;
        }
    }
}

void Nokia_5110::draw_line(uint8_t col0, uint8_t col1, uint8_t row0,
                           uint8_t row1, DrawMode mode) {
    int8_t x_mult = (col0 > col1) ? -1 : 1;
    int8_t y_mult = (row0 > row1) ? -1 : 1;
    uint8_t dx = abs(col1 - col0);
    uint8_t dy = abs(row1 - row0);

    if (dy < dx) {
        int8_t d = (2 * dy) - dx;
        uint8_t y = 0;
        for (uint8_t x = 0; x <= dx; x++) {
            draw_pixel(col0 + (x_mult * x), row0 + (y_mult * y), 1, mode);
            if (d > 0) {
                y++;
                d -= dx;
            }
            d += dy;
        }
    } else {
        int8_t d = (2 * dx) - dy;
        uint8_t x = 0;
        for (uint8_t y = 0; y <= dy; y++) {
            draw_pixel(col0 + (x_mult * x), row0 + (y_mult * y), 1, mode);
            if (d > 0) {
                x++;
                d -= dy;
            }
            d += dx;
        }
    }
}

void Nokia_5110::draw_rect(uint8_t col1, uint8_t row1, uint8_t col2,
                           uint8_t row2, FillMode fillMode, DrawMode drawMode) {
    for (uint8_t col = col1; col <= col2; col++) {
        for (uint8_t row = row1; row < row2; row++) {
            draw_pixel(col, row, get_fill_value(col, row, fillMode), drawMode);
        }
    }
}

uint8_t Nokia_5110::get_fill_value(uint8_t col, uint8_t row,
                                   FillMode fillMode) {
    switch (fillMode) {
    default:
    case solid:
        return 1;
    case none:
        return 0;
    case hatch:
        return (col + row) % 3 ? 0 : 1;
    case checkerboard:
        return (col + row) % 2;
    case stripes_horiz:
        return row % 2;
    case stripes_vert:
        return col % 2;
    }
}

// font from
// https://developer.mbed.org/users/eencae/code/N5110/docs/tip/N5110_8h_source.html
const uint8_t font[480] = {
    0x00, 0x00, 0x00, 0x00, 0x00, // (space)
    0x00, 0x00, 0x5F, 0x00, 0x00, // !
    0x00, 0x07, 0x00, 0x07, 0x00, // "
    0x14, 0x7F, 0x14, 0x7F, 0x14, // #
    0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
    0x23, 0x13, 0x08, 0x64, 0x62, // %
    0x36, 0x49, 0x55, 0x22, 0x50, // &
    0x00, 0x05, 0x03, 0x00, 0x00, // '
    0x00, 0x1C, 0x22, 0x41, 0x00, // (
    0x00, 0x41, 0x22, 0x1C, 0x00, // )
    0x08, 0x2A, 0x1C, 0x2A, 0x08, // *
    0x08, 0x08, 0x3E, 0x08, 0x08, // +
    0x00, 0x50, 0x30, 0x00, 0x00, // ,
    0x08, 0x08, 0x08, 0x08, 0x08, // -
    0x00, 0x60, 0x60, 0x00, 0x00, // .
    0x20, 0x10, 0x08, 0x04, 0x02, // /
    0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
    0x00, 0x42, 0x7F, 0x40, 0x00, // 1
    0x42, 0x61, 0x51, 0x49, 0x46, // 2
    0x21, 0x41, 0x45, 0x4B, 0x31, // 3
    0x18, 0x14, 0x12, 0x7F, 0x10, // 4
    0x27, 0x45, 0x45, 0x45, 0x39, // 5
    0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
    0x01, 0x71, 0x09, 0x05, 0x03, // 7
    0x36, 0x49, 0x49, 0x49, 0x36, // 8
    0x06, 0x49, 0x49, 0x29, 0x1E, // 9
    0x00, 0x36, 0x36, 0x00, 0x00, // :
    0x00, 0x56, 0x36, 0x00, 0x00, // ;
    0x00, 0x08, 0x14, 0x22, 0x41, // <
    0x14, 0x14, 0x14, 0x14, 0x14, // =
    0x41, 0x22, 0x14, 0x08, 0x00, // >
    0x02, 0x01, 0x51, 0x09, 0x06, // ?
    0x32, 0x49, 0x79, 0x41, 0x3E, // @
    0x7E, 0x11, 0x11, 0x11, 0x7E, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x7F, 0x41, 0x41, 0x22, 0x1C, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x7F, 0x09, 0x09, 0x01, 0x01, // F
    0x3E, 0x41, 0x41, 0x51, 0x32, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x20, 0x40, 0x41, 0x3F, 0x01, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x7F, 0x02, 0x04, 0x02, 0x7F, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O
    0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, // R
    0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x7F, 0x20, 0x18, 0x20, 0x7F, // W
    0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x03, 0x04, 0x78, 0x04, 0x03, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, // Z
    0x00, 0x00, 0x7F, 0x41, 0x41, // [
    0x02, 0x04, 0x08, 0x10, 0x20, // "\"
    0x41, 0x41, 0x7F, 0x00, 0x00, // ]
    0x04, 0x02, 0x01, 0x02, 0x04, // ^
    0x40, 0x40, 0x40, 0x40, 0x40, // _
    0x00, 0x01, 0x02, 0x04, 0x00, // `
    0x20, 0x54, 0x54, 0x54, 0x78, // a
    0x7F, 0x48, 0x44, 0x44, 0x38, // b
    0x38, 0x44, 0x44, 0x44, 0x20, // c
    0x38, 0x44, 0x44, 0x48, 0x7F, // d
    0x38, 0x54, 0x54, 0x54, 0x18, // e
    0x08, 0x7E, 0x09, 0x01, 0x02, // f
    0x08, 0x14, 0x54, 0x54, 0x3C, // g
    0x7F, 0x08, 0x04, 0x04, 0x78, // h
    0x00, 0x44, 0x7D, 0x40, 0x00, // i
    0x20, 0x40, 0x44, 0x3D, 0x00, // j
    0x00, 0x7F, 0x10, 0x28, 0x44, // k
    0x00, 0x41, 0x7F, 0x40, 0x00, // l
    0x7C, 0x04, 0x18, 0x04, 0x78, // m
    0x7C, 0x08, 0x04, 0x04, 0x78, // n
    0x38, 0x44, 0x44, 0x44, 0x38, // o
    0x7C, 0x14, 0x14, 0x14, 0x08, // p
    0x08, 0x14, 0x14, 0x18, 0x7C, // q
    0x7C, 0x08, 0x04, 0x04, 0x08, // r
    0x48, 0x54, 0x54, 0x54, 0x20, // s
    0x04, 0x3F, 0x44, 0x40, 0x20, // t
    0x3C, 0x40, 0x40, 0x20, 0x7C, // u
    0x1C, 0x20, 0x40, 0x20, 0x1C, // v
    0x3C, 0x40, 0x30, 0x40, 0x3C, // w
    0x44, 0x28, 0x10, 0x28, 0x44, // x
    0x0C, 0x50, 0x50, 0x50, 0x3C, // y
    0x44, 0x64, 0x54, 0x4C, 0x44, // z
    0x00, 0x08, 0x36, 0x41, 0x00, // {
    0x00, 0x00, 0x7F, 0x00, 0x00, // |
    0x00, 0x41, 0x36, 0x08, 0x00, // }
    0x08, 0x08, 0x2A, 0x1C, 0x08, // ->
    0x08, 0x1C, 0x2A, 0x08, 0x08  // <-
};
