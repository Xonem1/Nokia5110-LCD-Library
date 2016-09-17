#include "mbed-nokia-5110-lcd/Nokia_5110.h"

Nokia_5110 lcd(NC, D0, D1, D2, D3);
DigitalOut led(LED1);
InterruptIn increment(D5);
InterruptIn decrement(D6);
uint8_t contrast = 40;

void refreshDisplay() {
    lcd.setContrast(contrast);
    char buffer[10];
    itoa(contrast, buffer, 16);
    lcd.clearBuffer();
    lcd.printString((char*) "Contrast=0x", 0, 0);
    lcd.printString(buffer, 66, 0);
    lcd.display();
}

void blink() {
    led = 1;
    wait(0.25);
    led = 0;
}

void contrastUp() {
    if(contrast < 0x7F) contrast++;
    refreshDisplay();
    blink();
}

void contrastDown() {
    if (contrast > 0x00) contrast--;
    refreshDisplay();
    blink();
}

void app_start(int, char *[]) {
    increment.rise(&contrastUp);
    decrement.rise(&contrastDown);
    lcd.init(contrast, 4);
    lcd.clearBuffer();
    refreshDisplay();
}
