#ifndef OLED_SSD1306_SH1106_IMAGES_H
#define OLED_SSD1306_SH1106_IMAGES_H

#define espeasy_logo_width 32
#define espeasy_logo_height 32
const char espeasy_logo_bits[] PROGMEM= {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x10, 0x30, 0x00, 0x1c, 0x3c, 0x78, 0x00, 0x1e, 0x3e, 0x7c, 
    0x00, 0x0f, 0x1f, 0x3e, 0x80, 0x87, 0x0f, 0x1f, 0xc0, 0xc3, 0x87, 0x0f, 0xe0, 0xe1, 0xc3, 0x07, 
    0xf0, 0xf0, 0xe1, 0x03, 0x78, 0xf8, 0xf0, 0x01, 0x3c, 0x7c, 0xf8, 0x30, 0x1e, 0x3e, 0x7c, 0x78, 
    0x0c, 0x1f, 0x3e, 0x3c, 0x80, 0x0f, 0x1f, 0x1e, 0xc0, 0x87, 0x0f, 0x0f, 0xe0, 0xc3, 0x87, 0x07, 
    0xf0, 0xe1, 0xc3, 0x03, 0xf8, 0xf0, 0xe1, 0x01, 0x7c, 0xf8, 0xf0, 0x00, 0x3c, 0x7c, 0x78, 0x00, 
    0x1e, 0x3e, 0x3c, 0x00, 0x0c, 0x1f, 0x1e, 0x00, 0x80, 0x0f, 0x0f, 0x00, 0xc0, 0x87, 0x07, 0x00, 
    0xe0, 0xc3, 0x03, 0x1e, 0xf0, 0xe1, 0x01, 0x3f, 0xf8, 0xf0, 0x00, 0x7f, 0x7c, 0x78, 0x00, 0x7f, 
    0x3e, 0x3c, 0x00, 0x3f, 0x1e, 0x1c, 0x00, 0x1e, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const char activeSymbole[] PROGMEM = {
    0b00000000,
    0b00000000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b01000010,
    0b00100100,
    0b00011000
};

const char inactiveSymbole[] PROGMEM = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000
};

#endif