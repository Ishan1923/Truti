#ifndef SSD1306_HPP
#define SSD1306_HPP

#pragma once

#include "IDisplay.hpp"
#include "hal/II2C.hpp"

namespace drivers::display{
    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    class SSD1306 : public IDISPLAY {

        private:
            // Private members for SSD1306 specific implementation can be added here
            enum class Command : uint8_t {
                DisplayOff       = 0xAE,
                SetClockDiv      = 0xD5,
                SetMultiplex     = 0xA8,
                SetDisplayOffset = 0xD3,
                SetStartLine     = 0x40,
                ChargePump       = 0x8D,
                SetMemoryMode    = 0x20,
                SegmentRemap     = 0xA1,
                ComScanDec       = 0xC8,
                SetComPins       = 0xDA,
                SetContrast      = 0x81,
                SetPrecharge     = 0xD9,
                SetVcomDetect    = 0xDB,
                DisplayResumeRAM = 0xA4,
                NormalDisplay    = 0xA6,
                DisplayOn        = 0xAF
            };

            uint8_t SSD1306_Address = 0x3C,
                    CommandMode     = 0x00,
                    DataMode        = 0x40;


            II2C& i2c;

        public:

            SSD1306(II2C& i2c) : i2c(i2c){}

            DisplayStatus init() override;

            DisplayStatus setPixel(
                uint16_t* matrix) override;
            
            DisplayStatus clear() override;

            DisplayStatus update(uint16_t* matrix) override;

            DisplayStatus sendCommand(uint8_t cmd) override;

            DisplayStatus sendData(const uint8_t* data, size_t length) override;

    };

} // namespace drivers::display

#endif // SSD1306_HPP