#include <mbed.h>
#include <Nokia5110.h>

int main() {
    Nokia5110 display(p4, p3, p5, p6, p7);
    display.init(0x2C);
    display.clear_buffer();
    display.print_string("ABCDEFGHIJKLMN", 0, 0);
    display.print_string("abcdefghijklmn", 0, 8);
    display.print_string("!?@#$%^&*+()[]", 0, 16);
    display.fill_rrect(2, 28, 77, 38, 2);
    display.print_string("Hello World!", 4, 30, Nokia5110::pixel_xor);
    display.display();
}
