#ifndef IDISPLAY_HPP
#define IDISPLAY_HPP

#pragma once

#include <cstdint>
#include <cstddef>

class IDISPLAY{

    public:

        enum class DisplayStatus{
            ERROR,
            INVALID_ARGUMENTS,
            SUCCESS
        };

        virtual DisplayStatus init() = 0;

        virtual DisplayStatus setPixel(
            uint16_t* matrix) = 0;
        
        virtual DisplayStatus clear() = 0;

        virtual DisplayStatus update(uint16_t* matrix) = 0;

        virtual DisplayStatus sendCommand(uint8_t cmd) = 0;

        virtual DisplayStatus sendData(const uint8_t* data, size_t length) = 0;

        virtual ~IDISPLAY() = default;

};


#endif // IDISPLAY_HPP