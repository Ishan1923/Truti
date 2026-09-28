#include "utils/graphics/graphics.hpp"
#include "utils/fonts/font.hpp"
#include "drivers/display/IDisplay.hpp"

template<uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
void Graphics<ROWS, COLS, PAGES>::clear() {
    // Implementation for clearing the graphics context

    m_display.clear();

}

template<uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
void Graphics<ROWS, COLS, PAGES>::print(uint16_t x, uint16_t y, uint16_t space, uint16_t text_color, uint16_t text_size, const char* text) {
    // Implementation for printing a string starting at (x, y)

    cursorX = x;
    cursorY = y;

    while(*text) {
    
        if(*text < Font::firstChar || *text > Font::lastChar) {
            // Character is out of bounds, handle error or ignore
            ++text;
            continue;
        }

        if(*text == '\n'){
            cursorY += (Font::height + 1) * text_size;
            cursorX = x;
            ++text;
            continue;
        }

        if(cursorX + (Font::width + space) * text_size > COLS) {
            cursorX = 0;
            cursorY += (Font::height + 1) * text_size;
        }

        drawChar(cursorX, cursorY, *text, text_size, text_color);
        cursorX += (Font::width + space) * text_size;
    
        ++text;
    
    }

}

template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
void Graphics<ROWS, COLS, PAGES>::drawChar(uint16_t x, uint16_t y, char c, uint8_t size, uint16_t color) {
    // Implementation for drawing a character at (x, y)

    if (c < Font::firstChar || c > Font::lastChar) {
        // Character is out of bounds, handle error or ignore
        return;
    }

    const uint16_t charIndex = static_cast<uint16_t>(c - Font::firstChar);
    const uint16_t offset = charIndex * Font::width;

    for (uint8_t col = 0; col < Font::width; ++col) {
        uint8_t line = Font::fontData[offset + col];

        for (uint8_t row = 0; row < Font::height; ++row) {
            if (line & 0x01) {
                if (size == 1) {
                    m_display.drawPixel(x + col, y + row, color);
                }
                else{
                    fillRect(x + (col * size), y + (row * size), size, size, color);
                }
            }

            line >>= 1;
        }
    }
}

template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
void Graphics<ROWS, COLS, PAGES>::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    if (x >= COLS || y >= ROWS || x + width > COLS || y + height > ROWS) {
        return;
    }
    const uint16_t xEnd = x + width;
    const uint16_t yEnd = y + height;
    for(uint16_t i = x; i < xEnd; i++){
        for(uint16_t j = y; j < yEnd; j++){
            m_display.drawPixel(i, j, color);
        }
    }

    return;
}

// Explicit instantiation for your OLED dimensions
template class Graphics<64, 128, 8>;