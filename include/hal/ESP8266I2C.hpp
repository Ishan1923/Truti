#ifndef ESP8266I2C_HPP
#define ESP8266I2C_HPP

#pragma once

#include "II2C.hpp"
#include <Wire.h>

class ESP8266I2C : public II2C {

    private:

        TwoWire& wire;
        uint8_t address;

    public:

        ESP8266I2C(TwoWire& wire, uint8_t address);
        ~ESP8266I2C();

        I2CStatus init();

        I2CStatus write(
            uint8_t address,
            const uint8_t* data,
            uint32_t length
        ) override;

        I2CStatus read(
            uint8_t address,
            uint8_t* data,
            uint32_t length
        ) override;

        I2CStatus writeRead(
            uint8_t address,
            const uint8_t* writeData,
            uint32_t writeLength,
            uint8_t* readData,
            uint32_t readLength
        ) override;

};



#endif // ESP8266I2C_HPP