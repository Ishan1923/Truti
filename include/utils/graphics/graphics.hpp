#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cstdint>
#include <cstddef>

#include "drivers/display/IDisplay.hpp"

template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
class Graphics{

    IDISPLAY<ROWS, COLS, PAGES>& m_display;
    uint16_t cursorX = 0;
    uint16_t cursorY = 0;

    public:

        explicit Graphics(IDISPLAY<ROWS, COLS, PAGES>& display) : m_display(display) {}
        virtual ~Graphics() = default;
        virtual void clear();
        virtual void drawChar(uint16_t x, uint16_t y, char c, uint8_t size = 1, uint16_t color = 1);
        virtual void print(uint16_t x, uint16_t y, uint16_t space, uint16_t text_color, uint16_t text_size, const char* text);
        virtual void fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    };

#endif