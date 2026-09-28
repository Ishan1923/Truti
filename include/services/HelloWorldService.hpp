#ifndef HELLOWORLDSERVICE_HPP
#define HELLOWORLDSERVICE_HPP

#include "utils/graphics/graphics.hpp"

template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
class HelloWorldService
{
    Graphics<ROWS, COLS, PAGES>& sys_gfx;

    public:

        explicit HelloWorldService(Graphics<ROWS, COLS, PAGES>& gfx) : sys_gfx(gfx) {}

        void sayHello() {
            sys_gfx.clear();
            sys_gfx.print(0, 10, 1, 1, 1, "Hello\nWorld!");
        }
};

#endif // HELLOWORLDSERVICE_HPP