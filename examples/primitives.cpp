#include <mbed.h>
#include <Nokia5110.h>

int main() {
    Nokia5110 display(p4, p3, p5, p6, p7);
    display.init(0x2C);
    display.clear_buffer();
    display.fill_rrect(5, 5, 75, 18, 5, Nokia5110::pattern_black, Nokia5110::pixel_or);
    display.fill_circle(20, 25, 8, Nokia5110::pattern_black, Nokia5110::pixel_xor);
    display.draw_line(0, 40, 60, 30);
    display.fill_ellipse(40, 30, 10, 5, Nokia5110::pattern_dkgrey, Nokia5110::pixel_copy);
    display.draw_ellipse(40, 30, 10, 5, Nokia5110::pattern_black, Nokia5110::pixel_or);
    display.display();
}
