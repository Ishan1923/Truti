#include "drivers/display/SSD1306.hpp"
#include <cstring>

namespace drivers::display {

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    IDISPLAY::DisplayStatus SSD1306<ROWS, COLS, PAGES>::init() {
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
    
        return IDISPLAY::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    IDISPLAY::DisplayStatus SSD1306<ROWS, COLS, PAGES>::setPixel(uint16_t* matrix) {
        // Set pixels as per matrix data

        uint8_t buffer[COLS];
        for (uint16_t page = 0; page < PAGES; ++page) {
            for (uint16_t col = 0; col < COLS; ++col) {
                buffer[col] = (matrix[page * COLS + col]) ? 0xFF : 0x00; // Assuming matrix contains 1 for ON
            }
            sendData(buffer, COLS);
        }

        return IDISPLAY::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    IDISPLAY::DisplayStatus SSD1306<ROWS, COLS, PAGES>::clear() {
        // Clear the display


        uint8_t buffer[COLS] = {0};

        for (uint16_t page = 0; page < PAGES; ++page) {
            sendData(buffer, COLS);
        }


        return IDISPLAY::DisplayStatus::SUCCESS;
    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    IDISPLAY::DisplayStatus SSD1306<ROWS, COLS, PAGES>::update(uint16_t* matrix) {
        // Update the display with the current buffer


        clear();
        setPixel(matrix);

        return IDISPLAY::DisplayStatus::SUCCESS;
    }


    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    IDISPLAY::DisplayStatus SSD1306<ROWS, COLS, PAGES>::sendCommand(uint8_t cmd){

        uint8_t buff[2] = {0x00, cmd}; // Command mode
        i2c.write(SSD1306_Address, buff, 2);
        return IDISPLAY::DisplayStatus::SUCCESS;

    }

    template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
    IDISPLAY::DisplayStatus SSD1306<ROWS, COLS, PAGES>::sendData(const uint8_t* data, size_t length){

        if(length > COLS) {
            return IDISPLAY::DisplayStatus::INVALID_ARGUMENTS;
        }

        uint8_t buff[COLS + 1];
        buff[0] = 0x40;
        std::memcpy(buff + 1, data, length);   // length must be <= COLS
        i2c.write(SSD1306_Address, buff, length + 1);
        return IDISPLAY::DisplayStatus::SUCCESS;

    }


    template class SSD1306<64, 128, 8>; // Explicit instantiation for 64 rows, 128 columns, and 8 pages
    template class SSD1306<32, 128, 4>; // Explicit instantiation for 32 rows, 128 columns, and 4 pages
}