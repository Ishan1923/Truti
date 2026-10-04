#ifndef IMOTOR_HPP
#define IMOTOR_HPP

class IMotor{


    public:

        IMotor();

        virtual void moveForward();
        virtual void moveBackward();
        virtual void moveRight();
        virtual void moveLeft();
        virtual void moveDiagonalLeftUp();
        virtual void moveDiagonalLeftDown();
        virtual void moveDiagonalRightUp();
        virtual void moveDiagonalRightDown();



};


#endif // IMotor.hpp