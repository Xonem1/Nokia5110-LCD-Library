#include <mbed.h>
#include "Nokia5110.h"

Nokia5110 display(p4, p3, p5, p6, p7);
InterruptIn increment(p27);
InterruptIn decrement(p26);
uint8_t contrast = 40;

void refresh() {
    display.clear_buffer();
    display.set_contrast(contrast);
    char buffer[14] = "Contrast=0x  ";
    sprintf(buffer + 11, "%X", contrast);
    display.print_string(buffer, 0, 0);
    display.display();
}

void contrastUp() {
    if (contrast < 0x7F) {
        contrast++;
        refresh();
    }
}

void contrastDown() {
    if (contrast > 0x00) {
        contrast--;
        refresh();
    }
}

int main() {
    increment.rise(&contrastUp);
    decrement.rise(&contrastDown);
    display.init(contrast, 4);
    refresh();
}
