#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cstdint>
#include <cstddef>

class Graphics{


    public:

        virtual void clear() = 0;
        virtual void setPixel(uint16_t x, uint16_t y, bool value) = 0;
        virtual void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) = 0;
        virtual void drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;
        virtual void drawEllipse(uint16_t x, uint16_t y, uint16_t radius1, uint16_t radius2) = 0; // can be used to draw circles as well
        virtual void drawPolygon(const uint16_t* xPoints, const uint16_t* yPoints, size_t numPoints) = 0;
        virtual void drawText(uint16_t x, uint16_t y, const char* text) = 0;
};

#endif