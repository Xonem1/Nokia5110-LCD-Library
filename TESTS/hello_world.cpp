#include "Nokia5110.h"

int main(){
    Nokia5110 lcd(NC, D0, D1, D2, D3);
    lcd.init(0x40, 4);
    lcd.clear_buffer();
    lcd.print_string("Hello", 0, 0);
    lcd.print_string("World!", 3, 9);
    lcd.display();
}