#include "drivers/display/SSD1306.hpp"
#include <cstring>

namespace drivers::display {

    IDISPLAY::DisplayStatus SSD1306::init() {
        // Initialization code for SSD1306 display

        sendCommand(0xAE);                      // display off
        sendCommand(0xD5); sendCommand(0x80);   // clock divide
        sendCommand(0xA8); sendCommand(0x3F);   // multiplex ratio (128x64)
        sendCommand(0xD3); sendCommand(0x00);   // display offset
        sendCommand(0x40);                      // start line 0
        sendCommand(0x8D); sendCommand(0x14);   // charge pump ON — critical
        sendCommand(0x20); sendCommand(0x00);   // horizontal addressing mode
        sendCommand(0xA1);                      // segment remap
        sendCommand(0xC8);                      // COM scan direction
        sendCommand(0xDA); sendCommand(0x12);   // COM pins config
        sendCommand(0x81); sendCommand(0x7F);   // contrast
        sendCommand(0xD9); sendCommand(0xF1);   // precharge
        sendCommand(0xDB); sendCommand(0x40);   // VCOMH
        sendCommand(0xA4);                      // resume RAM content
        sendCommand(0xA6);                      // normal display
        sendCommand(0xAF);                      // display ON

        return IDISPLAY::DisplayStatus::SUCCESS;
    }

    IDISPLAY::DisplayStatus SSD1306::setPixel(uint16_t* matrix) {
        // Set pixels as per matrix data

        uint8_t buffer[cols_ * pages_];
        for (uint16_t page = 0; page < pages_; ++page) {
            for (uint16_t col = 0; col < cols_; ++col) {
                buffer[page * cols_ + col] = (matrix[page * cols_ + col]) ? 0xFF : 0x00; // Assuming matrix contains 1 for ON
            }
            sendData(buffer, cols_);
        }

        return IDISPLAY::DisplayStatus::SUCCESS;
    }

    IDISPLAY::DisplayStatus SSD1306::clear() {
        // Clear the display


        uint8_t buffer[cols_] = {0};

        for (uint16_t page = 0; page < pages_; ++page) {
            sendData(buffer, cols_);
        }


        return IDISPLAY::DisplayStatus::SUCCESS;
    }

    IDISPLAY::DisplayStatus SSD1306::update(uint16_t* matrix) {
        // Update the display with the current buffer


        clear();
        setPixel(matrix);

        return IDISPLAY::DisplayStatus::SUCCESS;
    }


    IDISPLAY::DisplayStatus SSD1306::sendCommand(uint8_t cmd){

        uint8_t buff[2] = {0x00, cmd}; // Command mode
        i2c.write(SSD1306_Address, buff, 2);
        return IDISPLAY::DisplayStatus::SUCCESS;

    }

    IDISPLAY::DisplayStatus SSD1306::sendData(const uint8_t* data, size_t length){

        uint8_t* buff = new uint8_t[length + 1];
        buff[0] = 0x40; // Data mode
        std::memcpy(buff + 1, data, length);
        i2c.write(SSD1306_Address, buff, length + 1);
        delete[] buff;
        return IDISPLAY::DisplayStatus::SUCCESS;

    }
}