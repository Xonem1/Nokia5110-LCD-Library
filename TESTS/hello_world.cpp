#include "Nokia5110.h"

int main(){
    Nokia_5110 lcd(NC, D0, D1, D2, D3);
    lcd.init(0x40, 4);
    lcd.clearBuffer();
    lcd.printString("Hello", 0, 0);
    lcd.printString("World!", 3, 9);
    lcd.display();
}