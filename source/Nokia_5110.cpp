#include "mbed-nokia-5110-lcd/Nokia_5110.h"

// the memory buffer for the LCD
uint8_t buffer[LCD_BYTES]; 

Nokia_5110::Nokia_5110(PinName sce, PinName rst, PinName dc, PinName dn, PinName sclk) {
   _lcdSPI = new SPI(dn, NC, sclk);
   _lcdSPI->format(LCD_SPI_BITS, LCD_SPI_MODE);
   _lcdSPI->frequency(LCD_SPI_FREQ);

   _sce = new DigitalOut(sce, 1);
   _rst = new DigitalOut(rst, 1);
   _dc  = new DigitalOut(dc, 0);
}

void Nokia_5110::init(uint8_t con, uint8_t bias) {
    reset();
    wait_ms(10);
    setContrast(con);
    setBias(bias);
    setMode(LCD_DISPLAYNORMAL);
}

void Nokia_5110::reset() {
    _rst->write(0);
    wait_ms(500);
    _rst->write(1);
}

void Nokia_5110::sendCommand(uint8_t cmd) {
    _sce->write(0);

    _lcdSPI->write(cmd);

    _sce->write(1);
}

void Nokia_5110::sendData(uint8_t data) {
    _dc->write(1);
    _sce->write(0);

    _lcdSPI->write(data);

    _sce->write(1);
    _dc->write(0);
}

void Nokia_5110::setContrast(uint8_t con) {
    if (con > 0x7f) con = 0x7f;

    sendCommand(LCD_FUNCTIONSET | LCD_EXTENDEDINSTRUCTION);
    sendCommand(LCD_SETVOP | con);
    sendCommand(LCD_FUNCTIONSET);
}

void Nokia_5110::setBias(uint8_t bias) {
    if (bias > 0x08) bias = 0x08;

    sendCommand(LCD_FUNCTIONSET | LCD_EXTENDEDINSTRUCTION);
    sendCommand(LCD_SETBIAS | bias);
    sendCommand(LCD_FUNCTIONSET);
}

void Nokia_5110::setMode(uint8_t mode) {
    if (mode > 0x08) mode = 0x08;

    sendCommand(LCD_DISPLAYCONTROL | mode);
}

void Nokia_5110::setPower(uint8_t pow) {
    pow = pow ? 0 : LCD_POWERDOWN;
    sendCommand(LCD_FUNCTIONSET | pow);
}

void Nokia_5110::setXAddr(uint8_t col) {
    col %= LCD_WIDTH;
    sendCommand(LCD_SETXADDR | col);
}

void Nokia_5110::setYAddr(uint8_t bank) {
    bank %= LCD_BANKS;
    sendCommand(LCD_SETYADDR | bank);
}

void Nokia_5110::setCursor(uint8_t col, uint8_t bank) {
    setXAddr(col);
    setYAddr(bank);
}

void Nokia_5110::clearBuffer() {
    for (unsigned int i = 0; i < LCD_BYTES; i++) {
        buffer[i] = 0x00;
    }
}

void Nokia_5110::display() {
    setYAddr(0);
    setXAddr(0);
    for (unsigned int i = 0; i < LCD_BYTES; i++) {
        sendData(buffer[i]);
    }
}

void Nokia_5110::drawPixel(uint8_t col, uint8_t row, uint8_t value, DrawMode drawMode) {
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

uint8_t Nokia_5110::getPixel(uint8_t col, uint8_t row) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;

    return buffer[col + (row / 8) * LCD_WIDTH] & (1 << (row % 8));
}

void Nokia_5110::drawByte(uint8_t col, uint8_t bank, uint8_t byte) {
    col %= LCD_WIDTH;
    bank %= LCD_BANKS;

    buffer[col + bank * LCD_WIDTH] = byte;
}

uint8_t Nokia_5110::getByte(uint8_t col, uint8_t bank) {
    col %= LCD_WIDTH;
    bank %= LCD_BANKS;

    return buffer[col + bank * LCD_WIDTH];
}

void Nokia_5110::printChar(char c, uint8_t col, uint8_t row, DrawMode mode) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;

    c -= 32;

    for (unsigned int i = 0; i < 5; i++) {
        for (unsigned int b = 0; b < 8; b++) {
            drawPixel(col + i, row + b, font[(5 * c) + i] & (1 << b), mode);
        }
    }
}

void Nokia_5110::printString(const char* str, uint8_t col, uint8_t row, DrawMode mode) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;

    while (*str && col + 6 < LCD_WIDTH) {
        printChar(*str, col, row, mode);
        col += 6;
        str++;
    }
}

void Nokia_5110::drawBitmap(const uint8_t* bmp, uint8_t col, uint8_t row, uint8_t width, uint8_t height, DrawMode mode) {
    uint8_t mask = 0x80;

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            drawPixel(col + x, row + y, *bmp & mask, mode);
            mask >>= 1;
            
            if (mask == 0) { //if we reached the end of the byte
                mask = 0x80;
                bmp++;
            }
        }
    } 
}

void Nokia_5110::drawWBitmap(const uint8_t* wbmp, uint8_t col, uint8_t row, DrawMode mode) {
    if (*wbmp++ != 0x00) return; //image type, only supports 0
    if (*wbmp++ != 0x00) return; //always 0
    uint8_t width = *wbmp++;     //image width in pixels
    uint8_t height = *wbmp++;    //image height in pixels

    uint8_t mask = 0x80;

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            drawPixel(col + x, row + y, *wbmp & mask, mode);
            mask >>= 1;

            if (mask == 0) { //if we reached the end of the byte
                mask = 0x80;
                wbmp++;
            }
        }
        mask = 0x80; //wbmps pad out the end of each row, so reset the mask
    } 
}

void Nokia_5110::drawRect(uint8_t col1, uint8_t row1, uint8_t col2, uint8_t row2, FillMode fillMode, DrawMode drawMode) {
    for (uint8_t col = col1; col <= col2; col++) {
        for (uint8_t row = row1; row < row2; row++) {
            drawPixel(col, row, getFillValue(col, row, fillMode), drawMode);
        }
    }
}

uint8_t Nokia_5110::getFillValue(uint8_t col, uint8_t row, FillMode fillMode) {
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