// =============================================================================
//  MPU6050 driver
//  6-axis motion sensor (3-axis accelerometer + 3-axis gyroscope + temperature)
//  talking over I2C.
// =============================================================================

#include "include/drivers/motion/IMotion.hpp"
#include "hal/II2C.hpp"
#include "hal/ITimer.hpp"

#include <cstdint>


// =============================================================================
// DECLARATIONS
// =============================================================================

class MPU6050 : public Motion {

private:

    // -------------------------------------------------------------------------
    //  Member variables
    // -------------------------------------------------------------------------

    // Used to talk to the sensor over the I2C bus
    II2C& i2c;

    // Used for waiting (e.g. delay after a sensor reset)
    ITimer& timer;

    // Currently selected measurement ranges.
    // The values are the raw register bits (0x00, 0x08, 0x10, 0x18).
    uint8_t accelRange = 0x00;   // 0x00 = +-2g
    uint8_t gyroRange = 0x00;    // 0x00 = +-250 deg/s

    //offset variables
    double accelOffsetX = 0.0;
    double accelOffsetY = 0.0;
    double accelOffsetZ = 0.0;

    double gyroOffsetX = 0.0;
    double gyroOffsetY = 0.0;
    double gyroOffsetZ = 0.0;

    //samples to take in one reading for average
    static constexpr uint16_t CALIBRATION_SAMPLES = 500;

    bool initialized = false;

    // -------------------------------------------------------------------------
    //  Constants: I2C address
    // -------------------------------------------------------------------------

    // I2C address of the MPU6050 (when its AD0 pin is low)
    static constexpr uint8_t MPU6050_ADDR = 0x68;

    // -------------------------------------------------------------------------
    //  Constants: MPU6050 register addresses
    //  (each one is the "address" of a setting or data value inside the chip)
    // -------------------------------------------------------------------------

    // Device identity register (should read back 0x68)
    static constexpr uint8_t WHO_AM_I_REG = 0x75;

    // FIFO buffer registers
    static constexpr uint8_t FIFO_R_W_REG = 0x74;
    static constexpr uint8_t FIFO_COUNT_H_REG = 0x72;
    static constexpr uint8_t FIFO_COUNT_L_REG = 0x73;

    // Power management registers (sleep, reset, clock source)
    static constexpr uint8_t PWR_MGMT_1_REG = 0x6B;
    static constexpr uint8_t PWR_MGMT_2_REG = 0x6C;

    // User control register (FIFO enable, I2C master enable, resets)
    static constexpr uint8_t USER_CTRL_REG = 0x6A;

    // Motion detection / signal path registers
    static constexpr uint8_t MOT_DETECT_CONTROL_REG = 0x69;
    static constexpr uint8_t SIGNAL_PATH_RESET_REG = 0x68;
    static constexpr uint8_t I2C_MST_DELAY_CTRL_REG = 0x67;

    // Data-out registers for I2C slave devices 0..3
    static constexpr uint8_t I2C_SLV3_D0_REG = 0x66;
    static constexpr uint8_t I2C_SLV2_D0_REG = 0x65;
    static constexpr uint8_t I2C_SLV1_D0_REG = 0x64;
    static constexpr uint8_t I2C_SLV0_D0_REG = 0x63;

    // Data read from external sensors attached to the MPU6050 (24 bytes)
    static constexpr uint8_t EXT_SENS_DATA_00_REG[24] = {
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
        0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
        0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
        0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60
    };

    // Gyroscope measurements: X high/low, Y high/low, Z high/low
    static constexpr uint8_t GYROSCOPE_MEASUREMENTS_REG[6] = {
        0x43, 0x44, 0x45, 0x46, 0x47, 0x48
    };

    // Temperature measurement (high byte and low byte)
    static constexpr uint8_t TEMP_OUT_L_REG = 0x42;
    static constexpr uint8_t TEMP_OUT_H_REG = 0x41;

    // Accelerometer measurements: X high/low, Y high/low, Z high/low
    static constexpr uint8_t ACCELEROMETER_MEASUREMENTS_REG[6] = {
        0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40
    };

    // Interrupt registers
    static constexpr uint8_t INTERRUPT_STATUS_REG = 0x3A;
    static constexpr uint8_t INTERRUPT_EN_REG = 0x38;
    static constexpr uint8_t INTERRUPT_PIN_CFG_REG = 0x37;

    // I2C master status
    static constexpr uint8_t I2C_MST_STATUS_REG = 0x36;

    // Control registers for I2C slave devices 4, 3, 2, 1, 0
    static constexpr uint8_t I2C_SLAVE4_CTRL_REG[5] = {
        0x31, 0x32, 0x33, 0x34, 0x35
    };

    static constexpr uint8_t I2C_SLAVE3_CTRL_REG[3] = {
        0x2E, 0x2F, 0x30
    };

    static constexpr uint8_t I2C_SLAVE2_CTRL_REG[3] = {
        0x2B, 0x2C, 0x2D
    };

    static constexpr uint8_t I2C_SLAVE1_CTRL_REG[3] = {
        0x28, 0x29, 0x2A
    };

    static constexpr uint8_t I2C_SLAVE0_CTRL_REG[3] = {
        0x25, 0x26, 0x27
    };

    // I2C master control, FIFO enable, motion threshold
    static constexpr uint8_t I2C_MST_CTRL_REG = 0x24;
    static constexpr uint8_t FIFO_EN_REG = 0x23;
    static constexpr uint8_t MOT_THR_REG = 0x1F;

    // Configuration registers (ranges, filter, sample rate)
    static constexpr uint8_t ACCEL_CONFIG_REG = 0x1C;
    static constexpr uint8_t GYRO_CONFIG_REG = 0x1B;
    static constexpr uint8_t CONFIG_REG = 0x1A;
    static constexpr uint8_t SAMPLE_RATE_DIV_REG = 0x19;

    // Self-test registers
    static constexpr uint8_t SELF_TEST_REG[4] = {
        0x0D, 0x0E, 0x0F, 0x10
    };


    // -------------------------------------------------------------------------
    //  Private functions: sensor operations
    // -------------------------------------------------------------------------

    // Check the sensor is present, reset it, wake it up and apply default settings
    II2C::I2CStatus initializeSensor();

    // Read all raw values from the sensor and convert them to physical units
    II2C::I2CStatus readSensorData();

    // Choose the accelerometer range (+-2g, +-4g, +-8g, +-16g)
    II2C::I2CStatus setAccelerometerRange(uint8_t range);

    // Choose the gyroscope range (+-250, +-500, +-1000, +-2000 deg/s)
    II2C::I2CStatus setGyroscopeRange(uint8_t range);

    // Set how often the sensor produces new samples
    II2C::I2CStatus setSampleRate(uint8_t rate);

    // Set the digital low-pass filter (DLPF) setting
    II2C::I2CStatus setFilterSettings(uint8_t settings);

    // How many raw counts equal 1 g, for the current accelerometer range
    double getAccelerometerSensitivity() const;

    // How many raw counts equal 1 deg/s, for the current gyroscope range
    double getGyroscopeSensitivity() const;

    II2C::I2CStatus calibrateSensor();
    // TODO
    void handleInterrupt();

    // Reset the sensor / put it to sleep / wake it up
    II2C::I2CStatus resetSensor();
    II2C::I2CStatus powerDownSensor();
    II2C::I2CStatus powerUpSensor();

    // TODO
    void configureInterrupts();

    // Read the 14 data bytes from the sensor and split them into raw values
    II2C::I2CStatus readRawData(
        int16_t& rawAccX,
        int16_t& rawAccY,
        int16_t& rawAccZ,
        int16_t& rawGyroX,
        int16_t& rawGyroY,
        int16_t& rawGyroZ,
        int16_t& rawTemp
    );

    // Turn raw counts into g, deg/s and degrees Celsius
    void convertRawDataToPhysicalUnits(
        int16_t rawAccX,
        int16_t rawAccY,
        int16_t rawAccZ,
        int16_t rawGyroX,
        int16_t rawGyroY,
        int16_t rawGyroZ,
        int16_t rawTemp
    );

    // -------------------------------------------------------------------------
    //  Private functions: low-level register access
    // -------------------------------------------------------------------------

    // Write one byte into a sensor register
    II2C::I2CStatus writeRegister(uint8_t reg, uint8_t value);

    // Read one byte from a sensor register
    II2C::I2CStatus readRegister(uint8_t reg, uint8_t& value);


public:

    // -------------------------------------------------------------------------
    //  Public functions
    // -------------------------------------------------------------------------

    // Create the driver, giving it the I2C bus and the timer to use
    explicit MPU6050(II2C& i2c, ITimer& timer);

    // Latest acceleration values (in g)
    double getAccX() const override;
    double getAccY() const override;
    double getAccZ() const override;

    // Latest rotation speeds (in degrees per second)
    double getGyroX() const override;
    double getGyroY() const override;
    double getGyroZ() const override;

    // Latest temperature (in degrees Celsius)
    double getTemperature() const override;

    II2C::I2CStatus initialize();
    II2C::I2CStatus update();
    II2C::I2CStatus calibrate();

};


// =============================================================================
// IMPLEMENTATIONS
// =============================================================================


// -----------------------------------------------------------------------------
//  Constructor
// -----------------------------------------------------------------------------

MPU6050::MPU6050(II2C& i2c, ITimer& timer)
    : i2c(i2c), timer(timer) {
}


// -----------------------------------------------------------------------------
//  Low-level register access
// -----------------------------------------------------------------------------

II2C::I2CStatus MPU6050::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};

    return i2c.write(MPU6050_ADDR, data, 2);

    /*
        It sends two bytes to the MPU6050:
        1. Register address.
        2. Value to write into that register.
        It also propagates the status returned by your I2C abstraction.
    */

}

II2C::I2CStatus MPU6050::readRegister(uint8_t reg, uint8_t& value) {
    return i2c.writeRead(MPU6050_ADDR, &reg, 1, &value, 1);

    /*
        How it works:
        1. Sends the register address (reg) to the MPU6050.
        2. Requests one byte from that register.
        3. Stores the received byte in value.
        4. Returns the communication status.
        Note: This relies on your existing ESP8266I2C::writeRead()
        implementation, whose repeated-start and error-propagation
        limitations we've marked for a later PR.
    */

}


// -----------------------------------------------------------------------------
//  Sensitivity helpers
//  (needed to convert raw sensor numbers into real units)
// -----------------------------------------------------------------------------

double MPU6050::getAccelerometerSensitivity() const {
    switch (accelRange) {
        case 0x00: return 16384.0;   // +-2g
        case 0x08: return 8192.0;    // +-4g
        case 0x10: return 4096.0;    // +-8g
        case 0x18: return 2048.0;    // +-16g
        default:   return 16384.0;
    }
}

double MPU6050::getGyroscopeSensitivity() const {
    switch (gyroRange) {
        case 0x00: return 131.0;     // +-250 deg/s
        case 0x08: return 65.5;      // +-500 deg/s
        case 0x10: return 32.8;      // +-1000 deg/s
        case 0x18: return 16.4;      // +-2000 deg/s
        default:   return 131.0;
    }
}


// -----------------------------------------------------------------------------
//  Initialization
// -----------------------------------------------------------------------------

II2C::I2CStatus MPU6050::initializeSensor() {
    uint8_t deviceID = 0;

    // Step 1: make sure the sensor answers and is really an MPU6050
    if (readRegister(WHO_AM_I_REG, deviceID) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    if (deviceID != 0x68) {
        return II2C::I2CStatus::ERROR;
    }

    // Step 2: reset the sensor, then give it time to restart
    if (resetSensor() != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    timer.delayMs(100);


    // Step 3: wake the MPU6050 by configuring its clock
    if(writeRegister(PWR_MGMT_1_REG, 0x01) != II2C::I2CStatus::SUCCESS){
        return II2C::I2CStatus::ERROR;
        // Why 0x01?
        // PWR_MGMT_1 uses:
        // - Bit 6: Sleep mode (0 = awake).
        // - Bits 2:0: Clock source (001 = PLL with X-axis gyroscope reference).
    }

    // Step 4: configuring the sensor, setting it initally to +-2g
    if (setAccelerometerRange(0x00) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    // Step 5: configuring the sensor, setting it initally to +-250 deg/s
    if (setGyroscopeRange(0x00) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    // Step 6: setting the sampling rate to 100Hz
    if (setSampleRate(9) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    // Step 7: Configuring digital lpf
    //DLPF_CFG = 3, which provides approximately:
    //- Accelerometer bandwidth: 44 Hz
    //- Gyroscope bandwidth: 42 Hz
    //- Gyroscope output rate: 1 kHz
    if (setFilterSettings(0x03) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    return II2C::I2CStatus::SUCCESS;
}


// -----------------------------------------------------------------------------
//  Configuration (ranges, sample rate, filter)
// -----------------------------------------------------------------------------

II2C::I2CStatus MPU6050::setAccelerometerRange(uint8_t range) {

    /*
        The function will:
        1. Read ACCEL_CONFIG_REG (0x1C).
        2. Preserve unrelated register bits.
        3. Replace bits 4 and 3 (the AFS_SEL field).
        4. Write the updated value back.
    */

    // Only the four valid range values are accepted
    if (range != 0x00 && range != 0x08 &&
        range != 0x10 && range != 0x18) {
        return II2C::I2CStatus::INVALID_ARGUMENTS;
    }

    uint8_t config = 0;

    if (readRegister(ACCEL_CONFIG_REG, config) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    config &= ~0x18;    // clear the range bits
    config |= range;    // set the new range bits

    II2C::I2CStatus status = writeRegister(ACCEL_CONFIG_REG, config);

    // Remember the range only if the write worked
    if (status == II2C::I2CStatus::SUCCESS) {
        accelRange = range;
    }

    return status;

}

II2C::I2CStatus MPU6050::setGyroscopeRange(uint8_t range) {

    // Only the four valid range values are accepted
    if (range != 0x00 && range != 0x08 &&
        range != 0x10 && range != 0x18) {
        return II2C::I2CStatus::INVALID_ARGUMENTS;
    }

    uint8_t config = 0;

    if (readRegister(GYRO_CONFIG_REG, config) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    config &= ~0x18;    // clear the range bits
    config |= range;    // set the new range bits

    II2C::I2CStatus status = writeRegister(GYRO_CONFIG_REG, config);

    // Remember the range only if the write worked
    if(status == II2C::I2CStatus::SUCCESS){
        gyroRange = range;
    }

    return status;

}

II2C::I2CStatus MPU6050::setSampleRate(uint8_t rate) {

    // f_s = f_gyro / (1 + SMPLRT_DIV)

    return writeRegister(SAMPLE_RATE_DIV_REG, rate);

}

II2C::I2CStatus MPU6050::setFilterSettings(uint8_t settings) {

    // The filter setting is a 3-bit value (0..7)
    if (settings > 0x07) {
        return II2C::I2CStatus::INVALID_ARGUMENTS;
    }

    uint8_t config = 0;

    if (readRegister(CONFIG_REG, config) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    config &= ~0x07;        // clear the filter bits
    config |= settings;     // set the new filter bits

    return writeRegister(CONFIG_REG, config);
}


// -----------------------------------------------------------------------------
//  Power control
// -----------------------------------------------------------------------------

II2C::I2CStatus MPU6050::resetSensor() {

    // Bit 7 of PWR_MGMT_1 = DEVICE_RESET
    return writeRegister(PWR_MGMT_1_REG, 0x80);

}

II2C::I2CStatus MPU6050::powerDownSensor() {

    uint8_t config = 0;

    if (readRegister(PWR_MGMT_1_REG, config) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    config |= 0x40;     // set the SLEEP bit

    return writeRegister(PWR_MGMT_1_REG, config);
}

II2C::I2CStatus MPU6050::powerUpSensor() {

    uint8_t config = 0;

    if (readRegister(PWR_MGMT_1_REG, config) != II2C::I2CStatus::SUCCESS) {
        return II2C::I2CStatus::ERROR;
    }

    config &= ~0x40;    // clear the SLEEP bit

    return writeRegister(PWR_MGMT_1_REG, config);
}


// -----------------------------------------------------------------------------
//  Reading measurements
// -----------------------------------------------------------------------------

II2C::I2CStatus MPU6050::readRawData(
    int16_t& rawAccX,
    int16_t& rawAccY,
    int16_t& rawAccZ,
    int16_t& rawGyroX,
    int16_t& rawGyroY,
    int16_t& rawGyroZ,
    int16_t& rawTemp
) {
    // The sensor stores 14 bytes in a row, starting at the accelerometer
    // X high byte: Acc X/Y/Z (6 bytes), Temp (2 bytes), Gyro X/Y/Z (6 bytes)
    uint8_t data[14] = {0};
    uint8_t startReg = ACCELEROMETER_MEASUREMENTS_REG[0];

    II2C::I2CStatus status =
        i2c.writeRead(MPU6050_ADDR, &startReg, 1, data, 14);

    if (status != II2C::I2CStatus::SUCCESS) {
        return status;
    }

    // Each value is 16 bits: combine the high byte and the low byte
    rawAccX = static_cast<int16_t>(
        (static_cast<uint16_t>(data[0]) << 8) | data[1]
    );

    rawAccY = static_cast<int16_t>(
        (static_cast<uint16_t>(data[2]) << 8) | data[3]
    );

    rawAccZ = static_cast<int16_t>(
        (static_cast<uint16_t>(data[4]) << 8) | data[5]
    );

    rawTemp = static_cast<int16_t>(
        (static_cast<uint16_t>(data[6]) << 8) | data[7]
    );

    rawGyroX = static_cast<int16_t>(
        (static_cast<uint16_t>(data[8]) << 8) | data[9]
    );

    rawGyroY = static_cast<int16_t>(
        (static_cast<uint16_t>(data[10]) << 8) | data[11]
    );

    rawGyroZ = static_cast<int16_t>(
        (static_cast<uint16_t>(data[12]) << 8) | data[13]
    );

    return II2C::I2CStatus::SUCCESS;
}

void MPU6050::convertRawDataToPhysicalUnits(
    int16_t rawAccX,
    int16_t rawAccY,
    int16_t rawAccZ,
    int16_t rawGyroX,
    int16_t rawGyroY,
    int16_t rawGyroZ,
    int16_t rawTemp
) {
    const double accelSensitivity = getAccelerometerSensitivity();
    const double gyroSensitivity = getGyroscopeSensitivity();

    // Convert acceleration to g
    accX = static_cast<double>(rawAccX) / accelSensitivity - accelOffsetX;
    accY = static_cast<double>(rawAccY) / accelSensitivity - accelOffsetY;
    accZ = static_cast<double>(rawAccZ) / accelSensitivity - accelOffsetZ;

    // Convert angular velocity to degrees per second
    gyroX = (static_cast<double>(rawGyroX) / gyroSensitivity) - gyroOffsetX;
    gyroY = (static_cast<double>(rawGyroY) / gyroSensitivity) - gyroOffsetY;
    gyroZ = (static_cast<double>(rawGyroZ) / gyroSensitivity) - gyroOffsetZ;

    // Convert temperature to degrees Celsius
    temperature = (static_cast<double>(rawTemp) / 340.0) + 36.53;
}

II2C::I2CStatus MPU6050::readSensorData() {
    int16_t rawAccX, rawAccY, rawAccZ;
    int16_t rawGyroX, rawGyroY, rawGyroZ;
    int16_t rawTemp;

    // Step 1: read the raw numbers from the sensor
    II2C::I2CStatus status = readRawData(
        rawAccX, rawAccY, rawAccZ,
        rawGyroX, rawGyroY, rawGyroZ,
        rawTemp
    );

    if (status != II2C::I2CStatus::SUCCESS) {
        return status;
    }

    // Step 2: convert them to g, deg/s and Celsius and store them
    convertRawDataToPhysicalUnits(
        rawAccX, rawAccY, rawAccZ,
        rawGyroX, rawGyroY, rawGyroZ,
        rawTemp
    );

    return II2C::I2CStatus::SUCCESS;
}

II2C::I2CStatus MPU6050::calibrateSensor() {
    double gyroSumX = 0.0;
    double gyroSumY = 0.0;
    double gyroSumZ = 0.0;

    for (uint16_t i = 0; i < CALIBRATION_SAMPLES; i++) {
        int16_t rawAccX, rawAccY, rawAccZ;
        int16_t rawGyroX, rawGyroY, rawGyroZ;
        int16_t rawTemp;

        II2C::I2CStatus status = readRawData(
            rawAccX, rawAccY, rawAccZ,
            rawGyroX, rawGyroY, rawGyroZ,
            rawTemp
        );

        if (status != II2C::I2CStatus::SUCCESS) {
            return status;
        }

        const double sensitivity = getGyroscopeSensitivity();

        gyroSumX += static_cast<double>(rawGyroX) / sensitivity;
        gyroSumY += static_cast<double>(rawGyroY) / sensitivity;
        gyroSumZ += static_cast<double>(rawGyroZ) / sensitivity;

        //delay only if any samples remain
        if (i < CALIBRATION_SAMPLES - 1) {
            timer.delayMs(10);
        }
    }

    gyroOffsetX = gyroSumX / CALIBRATION_SAMPLES;
    gyroOffsetY = gyroSumY / CALIBRATION_SAMPLES;
    gyroOffsetZ = gyroSumZ / CALIBRATION_SAMPLES;

    return II2C::I2CStatus::SUCCESS;
}


// -----------------------------------------------------------------------------
//  Public getters (return the values stored by the last readSensorData())
// -----------------------------------------------------------------------------

double MPU6050::getAccX() const {
    return accX;
}

double MPU6050::getAccY() const {
    return accY;
}

double MPU6050::getAccZ() const {
    return accZ;
}

double MPU6050::getGyroX() const {
    return gyroX;
}

double MPU6050::getGyroY() const {
    return gyroY;
}

double MPU6050::getGyroZ() const {
    return gyroZ;
}

double MPU6050::getTemperature() const {
    return temperature;
}

II2C::I2CStatus MPU6050::initialize(){
    initialized = false;
    II2C::I2CStatus status = initializeSensor();

    if (status == II2C::I2CStatus::SUCCESS) {
        initialized = true;
    }

    return status;
}

II2C::I2CStatus MPU6050::update(){
    if(!initialized){
        return II2C::I2CStatus::ERROR;
    }
    return readSensorData();
}

II2C::I2CStatus MPU6050::calibrate(){
    // Calibrates gyroscope offsets.
    // The sensor must remain stationary during calibration.
    
    if (!initialized) {
        return II2C::I2CStatus::ERROR;
    }
    
    return calibrateSensor();
}