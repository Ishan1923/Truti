#ifndef IMOTION_HPP
#define IMOTION_HPP


class Motion{

    protected:

        // acceleartion
        double accX = 0.0;    // longitudnal acceleration
        double accY = 0.0;    // lateral acceleration
        double accZ = 0.0;    // vertical acceleration

        // gyroscope - angular velocity
        double gyroX = 0.0;   // angular velocity around X-axis
        double gyroY = 0.0;   // angular velocity around Y-axis
        double gyroZ = 0.0;   // angular velocity around Z-axis

        // Thermodynamic Temperature
        double temperature = 0.0; // temperature in degrees Celsius


    public:

        Motion() = default;

        virtual double getAccX() const { return accX; }
        virtual double getAccY() const { return accY; }
        virtual double getAccZ() const { return accZ; }

        virtual double getGyroX() const { return gyroX; }
        virtual double getGyroY() const { return gyroY; }
        virtual double getGyroZ() const { return gyroZ; }

        virtual double getTemperature() const { return temperature; }

        virtual ~Motion() = default;


};


#endif // IMOTION_HPP   