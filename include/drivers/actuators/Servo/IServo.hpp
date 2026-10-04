#ifndef ISERVO_HPP
#define ISERVO_HPP

class IServo{

    protected:

        double LIMIT_DEG;
        double LIMIT_RAD;

    public:


        IServo(){
            LIMIT_DEG = 180;
            LIMIT_RAD = 3.14527;
        };

        virtual void rotate_cw_deg(double deg);
        virtual void rotate_cw_rad(double rad);
        virtual void rotate_acw_deg(double deg);
        virtual void rotate_acw_rad(double rad);


};

#endif //IServo.hpp