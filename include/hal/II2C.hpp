#ifndef II2C_HPP
#define II2C_HPP

#pragma once

#include <cstdint>
#include <cstddef>


class II2C {

    public:

        enum class I2CStatus{
            ERROR,
            INVALID_ARGUMENTS,
            SUCCESS
        };

        virtual I2CStatus write(
            uint8_t address,
            const uint8_t* data,
            uint32_t length
        ) = 0;

        virtual I2CStatus read(
            uint8_t address,
            uint8_t* data,
            uint32_t length
        ) = 0;

        virtual I2CStatus writeRead(
            uint8_t address,
            const uint8_t* writeData,
            uint32_t writeLength,
            uint8_t* readData,
            uint32_t readLength
        ) = 0;

        virtual ~II2C() = default;

};


#endif // I2C_HPP