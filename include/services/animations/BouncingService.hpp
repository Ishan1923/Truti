#ifndef ANIMATIONSERVICE_HPP
#define ANIMATIONSERVICE_HPP

#include "utils/graphics/graphics.hpp"

template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
class AnimationService {
private:
    Graphics<ROWS, COLS, PAGES>& m_gfx;
    
    // Position and velocity (using int16_t to allow negative speeds)
    int16_t x = 10;
    int16_t y = 10;
    int16_t dx = 2; // Moves 2 pixels right per frame
    int16_t dy = 2; // Moves 2 pixels down per frame
    int16_t size = 6; // Size of the bouncing box

public:
    AnimationService(Graphics<ROWS, COLS, PAGES>& gfx) : m_gfx(gfx) {}

    void nextFrame() {
        // 1. Wipe the internal memory buffer for the new frame
        m_gfx.clear();

        // 2. Update the box's position
        x += dx;
        y += dy;

        // 3. Check for collisions with the edges of the screen
        if (x <= 0 || x + size >= COLS) {
            dx = -dx; // Reverse X direction
        }
        if (y <= 0 || y + size >= ROWS) {
            dy = -dy; // Reverse Y direction
        }

        // 4. Draw the box at the new position
        m_gfx.fillRect(x, y, size, size, 1);
        
        // Optional: Draw some text that stays still while the box bounces!
        m_gfx.print(32, 28, 1, 1, 1, "Bouncing!");
    }
};

#endif