# Nokia5110-lcd  
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Build Status](https://travis-ci.org/drewcassidy/Nokia5110-LCD.svg?branch=master)](https://travis-ci.org/drewcassidy/Nokia5110-LCD)

This is a library for the LCD in the Nokia 5110/3310 cell phones. The LCD is a 48x84 black and white display, useful for
smartwatches and other small hobby electronics

###Files
- `src/`: source files
- `examples/`: example usage files
- `Doxyfile`: Doxygen config file

###Usage
See the examples readme for more info. The library is written for use with the mbed OS framework, but could easily be
modified for use with other platforms and frameworks. So far only tested with the NRF51822 chip. 

The display can be purchased on a breakout from [sparkfun](https://www.sparkfun.com/products/10168),
[adafruit](https://www.adafruit.com/product/338) or from various retailers on ebay or amazon. I've been unable to find the display
on its own without a breakout. 

The LCD runs at around 3.3V only, so you may need to use a voltage level shifter like the TXB0104 to hook it up to your microcontroller.
it should draw around 5mA on its own without backlight LEDs, depending on what is being displayed.