#include "nokia_5110.h"

// the memory buffer for the LCD
uint8_t buffer[LCD_WIDTH * LCD_HEIGHT / 8]; 

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