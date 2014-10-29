Arduino-OLED-Clock
==================

An analog Clock using an OLED display

There are two versions of this code for use on a 128x64 OLED display. The first version uses OLED libraries from ADAfruit. This library is easy to use but consumes 90% of dynamic memory.

The second version uses the U8GLIB library. I found this library a little more complicated to use at first, but this version uses only 48% of the dynamic memory.

Both sketches use just over 50% memory and there are some differences needed when 'printing' strings to the screen. There are links to the required libraries in both sketches as well as instructions for wiring the two modules.

The OLED used is one using a SSD1306 controller and 4 pins only (+5v, GND, SDA and SLC).
