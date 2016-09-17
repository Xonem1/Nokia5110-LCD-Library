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

void Nokia_5110::drawPixel(uint8_t col, uint8_t row, uint8_t value) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;
    if (value != 0) 
        buffer[col + (row / 8) * LCD_WIDTH] |= (1 << (row % 8));  
    else
        buffer[col + (row / 8) * LCD_WIDTH] &= ~(1 << (row % 8)); 
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

void Nokia_5110::printChar(char c, uint8_t col, uint8_t row) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;

    c -= 32;

    if (!(row % 8)) {
        for (unsigned int i = 0; i < 5; i++) {
            drawByte(col + i, row / 8, font[(5 * c) + i]);
        }
    } else {
        for (unsigned int i = 0; i < 5; i++) {
            for (unsigned int b = 0; b < 8; b++) {
                drawPixel(col + i, row + b, font[(5 * c) + i] & (1 << b));
            }
        }
    }
}

void Nokia_5110::printString(const char* str, uint8_t col, uint8_t row) {
    col %= LCD_WIDTH;
    row %= LCD_HEIGHT;

    while (*str && col + 6 < LCD_WIDTH) {
        printChar(*str, col, row);
        col += 6;
        str++;
    }
}