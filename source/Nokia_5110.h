#ifndef NOKIA_5110_H
#define NOKIA_5110_H

#include "mbed-drivers/mbed.h"

// 4MHz clock frequency, maximum of the display
#define LCD_SPI_FREQ 400000
// 8 bits per command/data
#define LCD_SPI_BITS 0x08
// Polarity 0 Phase 0, may be different on your platform
/*
 mode | POL PHA
 -----+--------
   0  |  0   0
   1  |  0   1
   2  |  1   0
   3  |  1   1
*/
#define LCD_SPI_MODE 0x00


#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define LCD_BANKS 6
#define LCD_BYTES 504

#define LCD_POWERDOWN 0x04
#define LCD_ENTRYMODE 0x02
#define LCD_EXTENDEDINSTRUCTION 0x01

#define LCD_DISPLAYBLANK 0x0
#define LCD_DISPLAYNORMAL 0x4
#define LCD_DISPLAYALLON 0x1
#define LCD_DISPLAYINVERTED 0x5

// basic instruction set
#define LCD_FUNCTIONSET 0x20
#define LCD_DISPLAYCONTROL 0x08
#define LCD_SETYADDR 0x40
#define LCD_SETXADDR 0x80

// extended instruction set
#define LCD_SETTEMP 0x04
#define LCD_SETBIAS 0x10
#define LCD_SETVOP 0x80

/**
 * @brief Mode for drawing pixels
 */
enum DrawMode {
    pixel_set,
    pixel_or,
    pixel_xor,
    pixel_clear
};

/**
 * @brief Bitmap draw options
 */
struct bmp_options_t {
    uint8_t rotation    : 2; /** < rotation of the bitmap in 90° increments, clockwise */
    uint8_t mirrorX     : 1; /** < mirror on the x-axis */
    uint8_t mirrorY     : 1; /** < mirror on the y-axis */
    uint8_t invert      : 1; /** < invert the bitmap's colors */
};

/**
 * @brief An API for using the Nokia 5110 display or other PCD8544-based displays with mbed-os
 * @details The Nokia 5110 display is a 84x48 pixel single-bit LCD using the PCD8544 controller.
 *  It is controlled by a modified version of the SPI protocol. 
 *  
 *  If the API or test files dont work at first, try changing the contrast setting. Different units
 *   will work best at different values. I've had this value range from 40 to 80
 * 
 */
class Nokia_5110 {
    public:
        /**
         * @brief constructor
         * 
         * @param sce Chip Enable pin
         * @param rst Reset pin
         * @param dc D/C pin
         * @param dn data pin (MOSI)
         * @param sclk clock pin (SCLK)
         */
        Nokia_5110(PinName sce, PinName rst, PinName dc, PinName dn, PinName sclk);

        /**
         * @brief initialize the display with given contrast and bias.
         *
         * @param con contrast for the display
         * @param bias bias for the display, should be 0x04 for the nokia 5110 display. only change for other PCD8544 displays
         */
        void init(uint8_t con = 40, uint8_t bias = 0x04);

        /**
         * @brief reset the display's memory
         */
        void reset();

        /**
         * @brief send a command to the display
         * 
         * @param cmd command to send
         */
        void sendCommand(uint8_t cmd);

        /**
         * @brief send a byte of data to the display
         * 
         * @param data data to send
         */
        void sendData(uint8_t data);

        /**
         * @brief sets the display's contrast
         * 
         * @param con contrast, usually between 40 and 60 depending on your display
         */
        void setContrast(uint8_t con);

        /**
         * @brief sets the dispay's bias
         * 
         * @param bias bias, should be 0x4 for the Nokia 5110 display
         */
        void setBias(uint8_t bias);

        /**
         * @brief sets the display's display mode
         * 
         * @param mode display mode (Blank, inverted, allon or normal)
         */
        void setMode(uint8_t mode);

        /**
         * @brief turns the display on or off
         * 
         * @param pow power, 0 = off, 1 = on
         */
        void setPower(uint8_t pow);

        /**
         * @brief sets the X value of the cursor
         * 
         * @param col x coordinate (0-83)
         */
        void setXAddr(uint8_t col);

        /**
         * @brief sets the Y value of the cursor
         * 
         * @param bank memory bank (0-5)
         */
        void setYAddr(uint8_t bank);

        /**
         * @brief sets the X and Y values of the cursor
         * 
         * @param col x coordinate (0-83)
         * @param bank memory bank (0-5)
         */
        void setCursor(uint8_t col, uint8_t bank);

        /**
         * @brief clears the screen buffer
         */
        void clearBuffer();

        /**
         * @brief sends the screen buffer to the display
         */
        void display();

        /**
         * @brief draws a pixel to the screen buffer 
         * 
         * @param col x coordinate (0-83)
         * @param row y coordinate (0-47)
         * @param value pixel value. 0 = white, 1 = black in normal mode
         * @param mode  draw mode (see above)
         */
        void drawPixel(uint8_t col, uint8_t row, uint8_t value, DrawMode mode = pixel_set);

        /**
         * @brief gets the value of a pixel from the screen buffer
         * 
         * @param col x coordinate (0-83)
         * @param row y coordinate (0-47)
         * 
         * @return value of the pixel, 0 if white
         */
        uint8_t getPixel(uint8_t col, uint8_t bank);

        /**
         * @brief draws a byte to the screen buffer
         * 
         * @param col x coordinate, (0-83)
         * @param bank memory bank (0-5)
         * @param byte byte to draw
         */
        void drawByte(uint8_t col, uint8_t bank, uint8_t byte);

        /**
         * @brief gets a byte from the screen buffer
         * 
         * @param col x coordinate (0-83)
         * @param bank memory bank (0-5)
         * 
         * @return byte from the screen buffer
         */
        uint8_t getByte(uint8_t col, uint8_t row);

        /**
         * @brief prints a 7x5 character to the screen buffer
         * 
         * @param c character to draw
         * @param col x coordinate of upper left (0-83)
         * @param row y coordinate of upper left (0-47)
         * @param mode  draw mode (see above)
         */
        void printChar(char c, uint8_t col, uint8_t row, DrawMode mode = pixel_set);

        /**
         * @brief prints a string to the screen buffer
         * 
         * @param str string to print
         * @param col x coordinate of upper left (0-83)
         * @param row y coordinate of upper left (0-47)
         * @param mode  draw mode (see above)
         */
        void printString(const char* str, uint8_t col, uint8_t row, DrawMode mode = pixel_set);

        /**
         * @brief draws a bitmap to the screen buffer
         *
         * @param bmp pointer to the start of the bitmap
         * @param col x coordinate of upper left (0-83)
         * @param row y coordinate of upper left (0-47)
         * @param width bitmap width in pixels
         * @param height bitmap height in pixels
         * @param options draw options 
         */
        void drawBitmap(const uint8_t* bmp, uint8_t col, uint8_t row, uint8_t width, uint8_t height, bmp_options_t options, DrawMode mode = pixel_set);

    private:
        SPI* _lcdSPI;

        DigitalOut* _sce;
        DigitalOut* _rst;
        DigitalOut* _dc;
    
};

//font from https://developer.mbed.org/users/eencae/code/N5110/docs/tip/N5110_8h_source.html
const uint8_t font[480] = {
    0x00, 0x00, 0x00, 0x00, 0x00,// (space)
    0x00, 0x00, 0x5F, 0x00, 0x00,// !
    0x00, 0x07, 0x00, 0x07, 0x00,// "
    0x14, 0x7F, 0x14, 0x7F, 0x14,// #
    0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
    0x23, 0x13, 0x08, 0x64, 0x62,// %
    0x36, 0x49, 0x55, 0x22, 0x50,// &
    0x00, 0x05, 0x03, 0x00, 0x00,// '
    0x00, 0x1C, 0x22, 0x41, 0x00,// (
    0x00, 0x41, 0x22, 0x1C, 0x00,// )
    0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
    0x08, 0x08, 0x3E, 0x08, 0x08,// +
    0x00, 0x50, 0x30, 0x00, 0x00,// ,
    0x08, 0x08, 0x08, 0x08, 0x08,// -
    0x00, 0x60, 0x60, 0x00, 0x00,// .
    0x20, 0x10, 0x08, 0x04, 0x02,// /
    0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
    0x00, 0x42, 0x7F, 0x40, 0x00,// 1
    0x42, 0x61, 0x51, 0x49, 0x46,// 2
    0x21, 0x41, 0x45, 0x4B, 0x31,// 3
    0x18, 0x14, 0x12, 0x7F, 0x10,// 4
    0x27, 0x45, 0x45, 0x45, 0x39,// 5
    0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
    0x01, 0x71, 0x09, 0x05, 0x03,// 7
    0x36, 0x49, 0x49, 0x49, 0x36,// 8
    0x06, 0x49, 0x49, 0x29, 0x1E,// 9
    0x00, 0x36, 0x36, 0x00, 0x00,// :
    0x00, 0x56, 0x36, 0x00, 0x00,// ;
    0x00, 0x08, 0x14, 0x22, 0x41,// <
    0x14, 0x14, 0x14, 0x14, 0x14,// =
    0x41, 0x22, 0x14, 0x08, 0x00,// >
    0x02, 0x01, 0x51, 0x09, 0x06,// ?
    0x32, 0x49, 0x79, 0x41, 0x3E,// @
    0x7E, 0x11, 0x11, 0x11, 0x7E,// A
    0x7F, 0x49, 0x49, 0x49, 0x36,// B
    0x3E, 0x41, 0x41, 0x41, 0x22,// C
    0x7F, 0x41, 0x41, 0x22, 0x1C,// D
    0x7F, 0x49, 0x49, 0x49, 0x41,// E
    0x7F, 0x09, 0x09, 0x01, 0x01,// F
    0x3E, 0x41, 0x41, 0x51, 0x32,// G
    0x7F, 0x08, 0x08, 0x08, 0x7F,// H
    0x00, 0x41, 0x7F, 0x41, 0x00,// I
    0x20, 0x40, 0x41, 0x3F, 0x01,// J
    0x7F, 0x08, 0x14, 0x22, 0x41,// K
    0x7F, 0x40, 0x40, 0x40, 0x40,// L
    0x7F, 0x02, 0x04, 0x02, 0x7F,// M
    0x7F, 0x04, 0x08, 0x10, 0x7F,// N
    0x3E, 0x41, 0x41, 0x41, 0x3E,// O
    0x7F, 0x09, 0x09, 0x09, 0x06,// P
    0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
    0x7F, 0x09, 0x19, 0x29, 0x46,// R
    0x46, 0x49, 0x49, 0x49, 0x31,// S
    0x01, 0x01, 0x7F, 0x01, 0x01,// T
    0x3F, 0x40, 0x40, 0x40, 0x3F,// U
    0x1F, 0x20, 0x40, 0x20, 0x1F,// V
    0x7F, 0x20, 0x18, 0x20, 0x7F,// W
    0x63, 0x14, 0x08, 0x14, 0x63,// X
    0x03, 0x04, 0x78, 0x04, 0x03,// Y
    0x61, 0x51, 0x49, 0x45, 0x43,// Z
    0x00, 0x00, 0x7F, 0x41, 0x41,// [
    0x02, 0x04, 0x08, 0x10, 0x20,// "\"
    0x41, 0x41, 0x7F, 0x00, 0x00,// ]
    0x04, 0x02, 0x01, 0x02, 0x04,// ^
    0x40, 0x40, 0x40, 0x40, 0x40,// _
    0x00, 0x01, 0x02, 0x04, 0x00,// `
    0x20, 0x54, 0x54, 0x54, 0x78,// a
    0x7F, 0x48, 0x44, 0x44, 0x38,// b
    0x38, 0x44, 0x44, 0x44, 0x20,// c
    0x38, 0x44, 0x44, 0x48, 0x7F,// d
    0x38, 0x54, 0x54, 0x54, 0x18,// e
    0x08, 0x7E, 0x09, 0x01, 0x02,// f
    0x08, 0x14, 0x54, 0x54, 0x3C,// g
    0x7F, 0x08, 0x04, 0x04, 0x78,// h
    0x00, 0x44, 0x7D, 0x40, 0x00,// i
    0x20, 0x40, 0x44, 0x3D, 0x00,// j
    0x00, 0x7F, 0x10, 0x28, 0x44,// k
    0x00, 0x41, 0x7F, 0x40, 0x00,// l
    0x7C, 0x04, 0x18, 0x04, 0x78,// m
    0x7C, 0x08, 0x04, 0x04, 0x78,// n
    0x38, 0x44, 0x44, 0x44, 0x38,// o
    0x7C, 0x14, 0x14, 0x14, 0x08,// p
    0x08, 0x14, 0x14, 0x18, 0x7C,// q
    0x7C, 0x08, 0x04, 0x04, 0x08,// r
    0x48, 0x54, 0x54, 0x54, 0x20,// s
    0x04, 0x3F, 0x44, 0x40, 0x20,// t
    0x3C, 0x40, 0x40, 0x20, 0x7C,// u
    0x1C, 0x20, 0x40, 0x20, 0x1C,// v
    0x3C, 0x40, 0x30, 0x40, 0x3C,// w
    0x44, 0x28, 0x10, 0x28, 0x44,// x
    0x0C, 0x50, 0x50, 0x50, 0x3C,// y
    0x44, 0x64, 0x54, 0x4C, 0x44,// z
    0x00, 0x08, 0x36, 0x41, 0x00,// {
    0x00, 0x00, 0x7F, 0x00, 0x00,// |
    0x00, 0x41, 0x36, 0x08, 0x00,// }
    0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
    0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
};

#endif