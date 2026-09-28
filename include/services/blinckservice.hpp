#ifndef BLINKSERVICE_HPP
#define BLINKSERVICE_HPP

#pragma once

#include "drivers/display/IDisplay.hpp"

template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
class BlinkService
{
    private:

        IDISPLAY<ROWS, COLS, PAGES>& display;


    public:

        enum class BlinkState{
            ON,
            OFF,
            INVALID,
            INVALID_ARGUMENTS
        };

        BlinkService(IDISPLAY<ROWS, COLS, PAGES>& display) : display(display) {}

        BlinkState blink(IDISPLAY<ROWS, COLS, PAGES>* display, uint16_t x, uint16_t y, uint16_t delayMs);



};
#endif // BLINKSERVICE_HPP