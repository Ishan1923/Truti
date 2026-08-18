#include "hal/ESP8266I2C.hpp"

namespace hal{
    
    ESP8266I2C::ESP8266I2C(TwoWire& wire, uint8_t address) : wire(wire), address(address) {}

    II2C::I2CStatus ESP8266I2C::init() {
        wire.begin();
        wire.setClock(400000); // Set I2C clock speed to 400kHz
        return II2C::I2CStatus::SUCCESS;
    }

    II2C::I2CStatus ESP8266I2C::write(
        uint8_t address,
        const uint8_t* data,
        size_t length 
    )
    {
        wire.beginTransmission(address);
        for(size_t i = 0; i < length; i++){
            wire.write(data[i]);
        }
        return wire.endTransmission() == 0 ? II2C::I2CStatus::SUCCESS : II2C::I2CStatus::ERROR;
    }

    II2C::I2CStatus ESP8266I2C::read(
        uint8_t address,
        uint8_t* data,
        size_t length
    )
    {
        wire.requestFrom(address, length);
        for(size_t i = 0; i < length; i++){
            if(wire.available()){
                data[i] = wire.read();
            } else {
                return II2C::I2CStatus::ERROR; // Not enough data available
            }
        }
        return II2C::I2CStatus::SUCCESS;
    }

    II2C::I2CStatus ESP8266I2C::writeRead(
        uint8_t address,
        const uint8_t* writeData,
        size_t writeLength,
        uint8_t* readData,
        size_t readLength
    )
    {
        // Implementation for write-read operation
        write(address, writeData, writeLength);
        read(address, readData, readLength);
        return II2C::I2CStatus::SUCCESS;
    }

    ESP8266I2C::~ESP8266I2C() {}

}