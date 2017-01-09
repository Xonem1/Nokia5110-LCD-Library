# Examples

###Files
- `bitmap.cpp`:
    demonstrates creating and drawing a raw bitmap to the display
- `contrast.cpp`:
    demonstrates changing changing the contrast of the LCD. This can be useful since the optimal contrast setting
    can change between units. Buttons should be connected on pins 26 and 27
- `primitives.cpp`:
    demonstrates drawing various geometric primitives to the display, as well as draw modes and patterns
- `text.cpp`:
    demonstrates drawing text to the display, to showcase what several characters look like and that text doesnt have to be
    alligned with one of the 6 memory banks, unlike other Nokia5110 display libraries

###Hookup
The default hookup for these examples are:
- pin 3 to RST
- pin 4 to SCE/CE
- pin 5 to DC
- pin 6 to DN/MOSI
- pin 7 to CLK

This is just how my board is setup. The examples are easily modfied to change this. keep in mind the display is meant for 3.3V logic, so connect it through a level shifter or 5k resistors to prevent damaging it