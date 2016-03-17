#include "source/Nokia_5110.h"

void app_start(int, char *[]) {
    Nokia_5110 lcd(NC, D0, D1, D2, D3);
    lcd.init(40, 4);
    lcd.clearBuffer();
    lcd.printString("Hello", 0, 0);
    lcd.printString("World!", 0, 1);
    lcd.display();
}