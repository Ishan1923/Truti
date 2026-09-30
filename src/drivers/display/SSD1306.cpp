#include "drivers/display/SSD1306.hpp"
#include <cstring>

namespace drivers::display {

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    typename IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus
    SSD1306<ROWS, COLS, PAGES>::init() {
        sendCommand(static_cast<uint8_t>(Command::DisplayOff));
    
        sendCommand(static_cast<uint8_t>(Command::SetClockDiv));
        sendCommand(0x80);
    
        sendCommand(static_cast<uint8_t>(Command::SetMultiplex));
        sendCommand(PAGES * 8 - 1);              // multiplex ratio derived from PAGES, not hardcoded 0x3F
    
        sendCommand(static_cast<uint8_t>(Command::SetDisplayOffset));
        sendCommand(0x00);
    
        sendCommand(static_cast<uint8_t>(Command::SetStartLine));   // 0x40 itself IS the command+arg combined
    
        sendCommand(static_cast<uint8_t>(Command::ChargePump));
        sendCommand(0x14);
    
        sendCommand(static_cast<uint8_t>(Command::SetMemoryMode));
        sendCommand(0x00);                        // horizontal addressing mode
    
        sendCommand(static_cast<uint8_t>(Command::SegmentRemap));
        sendCommand(static_cast<uint8_t>(Command::ComScanDec));
    
        sendCommand(static_cast<uint8_t>(Command::SetComPins));
        sendCommand(0x12);
    
        sendCommand(static_cast<uint8_t>(Command::SetContrast));
        sendCommand(0x7F);
    
        sendCommand(static_cast<uint8_t>(Command::SetPrecharge));
        sendCommand(0xF1);
    
        sendCommand(static_cast<uint8_t>(Command::SetVcomDetect));
        sendCommand(0x40);
    
        sendCommand(static_cast<uint8_t>(Command::DisplayResumeRAM));
        sendCommand(static_cast<uint8_t>(Command::NormalDisplay));
        sendCommand(static_cast<uint8_t>(Command::DisplayOn));
    
        return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    typename IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus
    SSD1306<ROWS, COLS, PAGES>::setPixel(uint16_t* matrix) {
        uint8_t buffer[COLS];
        for (uint16_t page = 0; page < PAGES; ++page) {
            for (uint16_t col = 0; col < COLS; ++col) {
                buffer[col] = (matrix[page * COLS + col]) ? 0xFF : 0x00;
            }
            sendData(buffer, COLS);
        }

        return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    typename IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus SSD1306<ROWS, COLS, PAGES>::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
        if (x >= COLS || y >= ROWS) return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::INVALID_ARGUMENTS;

        uint16_t index = x + ((y / 8) * COLS);
        uint8_t bit = y % 8;

        if (color) {
            buffer[index] |= (1 << bit);
        } else {
            buffer[index] &= ~(1 << bit);
        }
        return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    typename IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus SSD1306<ROWS, COLS, PAGES>::clear() {

        // Instantly wipe the local RAM array with zeros
        std::memset(buffer, 0, sizeof(buffer));

        return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    typename IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus SSD1306<ROWS, COLS, PAGES>::update(uint16_t* matrix) {

        // Reset pointers to the top-left of the screen
        sendCommand(0x21); // Column Address
        sendCommand(0x00);
        sendCommand(COLS - 1);

        sendCommand(0x22); // Page Address
        sendCommand(0x00);
        sendCommand(PAGES - 1);

        // Send the buffer in safe 16-byte chunks!
        for (uint16_t i = 0; i < sizeof(buffer); i += 16) {
            uint8_t chunk[17];
            chunk[0] = 0x40; // 0x40 means "Pixel Data follows"
            std::memcpy(chunk + 1, &buffer[i], 16);

            // Use your custom HAL to send the 17-byte chunk
            i2c.write(SSD1306_Address, chunk, 17);
        }

        return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    typename IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus
    SSD1306<ROWS, COLS, PAGES>::sendCommand(uint8_t cmd){
        uint8_t buff[2] = {0x00, cmd};
        i2c.write(SSD1306_Address, buff, 2);
        return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    typename IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus
    SSD1306<ROWS, COLS, PAGES>::sendData(const uint8_t* data, size_t length){
        if(length > COLS) {
            return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::INVALID_ARGUMENTS;
        }

        uint8_t buff[COLS + 1];
        buff[0] = 0x40;
        std::memcpy(buff + 1, data, length);
        i2c.write(SSD1306_Address, buff, length + 1);
        return IDISPLAY<ROWS, COLS, PAGES>::DisplayStatus::SUCCESS;
    }

    template class SSD1306<64, 128, 8>;
    template class SSD1306<32, 128, 4>;
}