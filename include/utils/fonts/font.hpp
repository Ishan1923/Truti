#ifndef FONT_HPP
#define FONT_HPP

#include <cstdint>

class Font {
    public:
        static const uint8_t width; // Width of each character in pixels
        static const uint8_t height; // Height of each character in pixels
        static const uint8_t firstChar; // ASCII value of the first character in the font
        static const uint8_t lastChar; // ASCII value of the last character in the font

        static const uint8_t fontData[]; // Single array for all characters, each character is represented by a series of bytes (one byte per row)
                                         // This will be a 2D array flattened into a 1D array, where each character's data is stored sequentially.
                                         // and at the implementation side, we will just use an offset to access the correct character data based on 
                                         // its ASCII value.

};



#endif // FONT1_HPP