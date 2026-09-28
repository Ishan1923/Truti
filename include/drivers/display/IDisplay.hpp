#ifndef IDISPLAY_HPP
#define IDISPLAY_HPP

#pragma once

#include <cstdint>
#include <cstddef>

template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES> // important: these things have to be fixed at compile time
class IDISPLAY{

protected:
    uint8_t buffer[PAGES][COLS] = {0};

    public:

        enum class DisplayStatus{
            ERROR,
            INVALID_ARGUMENTS,
            SUCCESS
        };

        virtual DisplayStatus init() = 0;

        virtual DisplayStatus setPixel(
            uint16_t* matrix) = 0;
        
        virtual DisplayStatus drawPixel (uint16_t x, uint16_t y, uint16_t color) = 0;
        
        virtual DisplayStatus clear() = 0;

        virtual DisplayStatus update(uint16_t* matrix) = 0;

        virtual DisplayStatus sendCommand(uint8_t cmd) = 0;

        virtual DisplayStatus sendData(const uint8_t* data, size_t length) = 0;

        virtual ~IDISPLAY() = default;

};


#endif // IDISPLAY_HPP