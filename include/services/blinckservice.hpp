#ifndef BLINKSERVICE_HPP
#define BLINKSERVICE_HPP

#pragma once

#include "drivers/display/IDisplay.hpp"

class BlinkService
{
    private:

        IDISPLAY& display;


    public:

        enum class BlinkState{
            ON,
            OFF,
            INVALID,
            INVALID_ARGUMENTS
        };

        BlinkService(IDISPLAY& display) : display(display) {}

        BlinkState blink(IDISPLAY* display, uint16_t x, uint16_t y, uint16_t delayMs);



};
#endif // BLINKSERVICE_HPP