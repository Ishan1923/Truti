#ifndef IMOTION_HPP
#define IMOTION_HPP


class Motion{

    protected:

        // acceleartion
        double accX;    // longitudnal acceleration
        double accY;    // lateral acceleration
        double accZ;    // vertical acceleration

        // gyroscope - angular velocity
        double gyroX;   // angular velocity around X-axis
        double gyroY;   // angular velocity around Y-axis
        double gyroZ;   // angular velocity around Z-axis

        // Thermodynamic Temperature
        double temperature; // temperature in degrees Celsius


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