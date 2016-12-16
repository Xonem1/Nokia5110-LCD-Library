#ifndef NOKIA5110_H
#define NOKIA5110_H

#include "mbed.h"

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
enum DrawMode { pixel_set, pixel_or, pixel_xor, pixel_clear };

/**
 * @brief Mode for filling shapes
 */
enum FillMode { solid, none, hatch, checkerboard, stripes_horiz, stripes_vert };

/**
 * @brief An API for using the Nokia 5110 display or other PCD8544-based
 * displays with mbed-os
 * @details The Nokia 5110 display is a 84x48 pixel single-bit LCD using the
 * PCD8544 controller.
 *  It is controlled by a modified version of the SPI protocol.
 *
 *  If the API or test files dont work at first, try changing the contrast
 * setting. Different units
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
           * @param bias bias for the display, should be 0x04 for the nokia 5110
     * display. only change for other PCD8544 displays
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
           * @param con contrast, usually between 40 and 60 depending on your
     * display
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
           * @param drawMode  draw mode (see above)
           */
    void drawPixel(uint8_t col, uint8_t row, uint8_t value,
                   DrawMode drawMode = pixel_set);

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
    void printString(const char *str, uint8_t col, uint8_t row,
                     DrawMode mode = pixel_set);

    /**
           * @brief draws a bitmap to the screen buffer in an unpadded format
           *
           * @param bmp pointer to the start of the bitmap
           * @param col x coordinate of upper left (0-83)
           * @param row y coordinate of upper left (0-47)
           * @param width bitmap width in pixels
           * @param height bitmap height in pixels
           */
    void drawBitmap(const uint8_t *bmp, uint8_t col, uint8_t row, uint8_t width,
                    uint8_t height, DrawMode mode = pixel_set);

    /**
           * @brief draws a bitmap to the screen buffer in the WBMP format
           *
           * @param wbmp pointer to the start of the bitmap
           * @param col x coordinate of upper left (0-83)
           * @param row y coordinate of upper left (0-47)
           */
    void drawWBitmap(const uint8_t *wbmp, uint8_t col, uint8_t row,
                     DrawMode mode = pixel_set);

    /**
           * @brief draws a filled rectangle
           *
           * @param col1 column of the first point
           * @param row1 row of the first point
           * @param col2 column of the second point
           * @param row2 row of the second point
           * @param mode  draw mode (see above)
           */
    void drawRect(uint8_t col1, uint8_t row1, uint8_t col2, uint8_t row2,
                  FillMode fillMode = solid, DrawMode drawMode = pixel_set);

  private:
    SPI *_lcdSPI;

    DigitalOut *_sce;
    DigitalOut *_rst;
    DigitalOut *_dc;

    uint8_t getFillValue(uint8_t col, uint8_t row, FillMode fillMode);
};

extern const uint8_t font[480];

#endif