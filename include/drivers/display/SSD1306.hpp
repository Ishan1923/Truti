#ifndef SSD1306_HPP
#define SSD1306_HPP

#pragma once

#include "IDisplay.hpp"
#include "hal/II2C.hpp"


class SSD1306 : public IDISPLAY {

    private:
        // Private members for SSD1306 specific implementation can be added here
        enum class Command : uint8_t {
            DisplayOff      = 0xAE,
            DisplayOn       = 0xAF,
            SetClockDiv     = 0xD5,
            SetMultiplex    = 0xA8,
            SetDisplayOffset= 0xD3,
            ChargePump      = 0x8D,
            SetMemoryMode   = 0x20,
            SetContrast     = 0x81,
            NormalDisplay   = 0xA6
        };

        uint8_t SSD1306_Address = 0x3C,
                CommandMode     = 0x00,
                DataMode        = 0x40;

        
        II2C& i2c;

        uint16_t rows_;
        uint16_t cols_;
        uint16_t pages_;

    public:

        SSD1306(II2C& i2c, uint16_t rows = 64, uint16_t cols = 128, uint16_t pages = 8) : i2c(i2c), rows_(rows), cols_(cols), pages_(pages) {}

        DisplayStatus init() override;

        DisplayStatus setPixel(
            uint16_t* matrix) override;
        
        DisplayStatus clear() override;

        DisplayStatus update(uint16_t* matrix) override;

        DisplayStatus sendCommand(uint8_t cmd) override;

        DisplayStatus sendData(const uint8_t* data, size_t length) override;

};

#endif // SSD1306_HPP