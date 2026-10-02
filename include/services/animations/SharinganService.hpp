// /* ==============================================================================
//  * SHARINGAN ANIMATION & GRAPHICS ENGINE ARCHITECTURE
//  * ==============================================================================
//  *
//  * --- PART 1: THE MATHEMATICS ---
//  * 1. The Sharingan Blades (Polar to Cartesian)
//  *    The 3 blades are offset by 120 degrees (2.094 radians). To form the curved 
//  *    "scythe" shape, we stamp 9 overlapping filled circles per blade, steadily 
//  *    increasing the radius while slightly bending the angle. We convert these 
//  *    polar coordinates back to screen pixels (x, y) using standard trigonometry:
//  *      x = center_x + (radius * cos(angle))
//  *      y = center_y + (radius * sin(angle))
//  *
//  * 2. The Blinking Eye (Parabolas & Masking)
//  *    The almond shape of the eye socket is modeled using an inverted parabola 
//  *    equation: 1 - x^2. To animate the blink, we multiply the parabola's max height 
//  *    by 'eyeOpenFactor' (scaling from 1.0 to 0.0). We then use "Masking": drawing 
//  *    solid black rectangles (color = 0) from the screen edges to the eyelid 
//  *    curves, digitally erasing any part of the Sharingan drawn outside the eye.
//  *
//  * 3. Bresenham's Midpoint Circle Algorithm
//  *    Used for the un-filled iris outline. It draws perfect circles using only 
//  *    lightning-fast integer math, completely avoiding heavy floating-point 
//  *    operations by calculating 1/8th of the circle and mirroring it 8 ways.
//  *
//  * --- PART 2: THE STACK EXECUTION (TOP-TO-BOTTOM) ---
//  * How the math above actually becomes physical light on the OLED panel:
//  * 
//  * 1. Service Layer (SharinganService): Computes the math and shapes, then passes 
//  *    them to the Graphics Engine via m_gfx.fillRect() or clear().
//  * 2. Graphics Engine (Graphics): Translates abstract shapes and text down into 
//  *    hundreds of individual drawPixel(x, y, color) requests.
//  * 3. Display Driver (SSD1306): Takes the (x, y) coordinates, calculates the exact 
//  *    1D index in its local 1024-byte RAM buffer, and uses bitwise math (|= or &=) 
//  *    to toggle that specific bit ON or OFF in memory.
//  * 4. Hardware Abstraction (ESP8266I2C): When display.update() is called, it slices 
//  *    the 1024-byte RAM buffer into 16-byte chunks (to bypass ESP8266 hardware 
//  *    buffer limits) and blasts them over the physical I2C pins to the OLED.
//  * ============================================================================== */


// #ifndef SHARINGAN_SERVICE_HPP
// #define SHARINGAN_SERVICE_HPP

// #include "utils/graphics/graphics.hpp"
// #include <cmath> 

// template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
// class SharinganService {
// private:
//     Graphics<ROWS, COLS, PAGES>& m_gfx;
//     float rotation = 0.0f;
    
//     // Blink state machine variables
//     int blinkTimer = 0;
//     bool isBlinking = false;
//     bool isClosing = false;
//     float eyeOpenFactor = 1.0f; // 1.0 = fully open, 0.0 = fully closed

//     void fillCircle(int cx, int cy, int r) {
//         for(int y = -r; y <= r; y++) {
//             for(int x = -r; x <= r; x++) {
//                 if(x*x + y*y <= r*r) {
//                     if (cx+x >= 0 && cx+x < COLS && cy+y >= 0 && cy+y < ROWS) {
//                         m_gfx.fillRect(cx+x, cy+y, 1, 1, 1); 
//                     }
//                 }
//             }
//         }
//     }

//     void drawCircle(int cx, int cy, int r) {
//         int x = 0, y = r, p = 3 - 2 * r;
//         while (y >= x) {
//             m_gfx.fillRect(cx + x, cy + y, 1, 1, 1);
//             m_gfx.fillRect(cx - x, cy + y, 1, 1, 1);
//             m_gfx.fillRect(cx + x, cy - y, 1, 1, 1);
//             m_gfx.fillRect(cx - x, cy - y, 1, 1, 1);
//             m_gfx.fillRect(cx + y, cy + x, 1, 1, 1);
//             m_gfx.fillRect(cx - y, cy + x, 1, 1, 1);
//             m_gfx.fillRect(cx + y, cy - x, 1, 1, 1);
//             m_gfx.fillRect(cx - y, cy - x, 1, 1, 1);
//             x++;
//             if (p > 0) {
//                 y--;
//                 p = p + 4 * (x - y) + 10;
//             } else {
//                 p = p + 4 * x + 6;
//             }
//         }
//     }

// public:
//     SharinganService(Graphics<ROWS, COLS, PAGES>& gfx) : m_gfx(gfx) {}

//     void nextFrame() {
//         m_gfx.clear();

//         int cx = COLS / 2;
//         int cy = ROWS / 2;

//         // ==========================================
//         // 1. DRAW THE SHARINGAN (The Eyeball)
//         // ==========================================
//         drawCircle(cx, cy, 28); 
//         drawCircle(cx, cy, 27); 
//         fillCircle(cx, cy, 4);
        
//         for (int b = 0; b < 3; b++) {
//             float base_angle = rotation + (b * 2.09439f);
//             int bladeThickness[] = {3, 4, 5, 5, 4, 3, 2, 1, 1}; 
            
//             for (int i = 0; i < 9; i++) {
//                 float radiusFromCenter = 6.0f + (i * 2.5f); 
//                 float curve_angle = base_angle + (i * 0.15f); 
                
//                 int px = cx + (int)(radiusFromCenter * cos(curve_angle));
//                 int py = cy + (int)(radiusFromCenter * sin(curve_angle));
                
//                 fillCircle(px, py, bladeThickness[i]);
//             }
//         }

//         // ==========================================
//         // 2. CALCULATE BLINK ANIMATION
//         // ==========================================
//         if (isBlinking) {
//             if (isClosing) {
//                 eyeOpenFactor -= 0.15f; // Speed of closing
//                 if (eyeOpenFactor <= 0.0f) {
//                     eyeOpenFactor = 0.0f;
//                     isClosing = false; // Start opening
//                 }
//             } else {
//                 eyeOpenFactor += 0.15f; // Speed of opening
//                 if (eyeOpenFactor >= 1.0f) {
//                     eyeOpenFactor = 1.0f;
//                     isBlinking = false; // Done blinking
//                 }
//             }
//         } else {
//             blinkTimer++;
//             // Trigger a blink roughly every 150 frames (couple of seconds)
//             if (blinkTimer > 150) {
//                 isBlinking = true;
//                 isClosing = true;
//                 blinkTimer = 0;
//             }
//         }

//         // ==========================================
//         // 3. DRAW THE EYELIDS (Masking with Black)
//         // ==========================================
//         int eyeWidth = 46; 
//         int maxEyeHeight = 29;

//         for (int x = cx - eyeWidth; x <= cx + eyeWidth; x++) {
//             // Calculate a parabolic curve for the eye shape
//             float dx = (float)abs(x - cx) / eyeWidth;
//             float curve = 1.0f - (dx * dx); // inverted parabola
            
//             // Apply the blink factor to close the eye
//             int currentHeight = (int)(maxEyeHeight * curve * eyeOpenFactor);

//             // Calculate the Y coordinates of the top and bottom eyelids
//             int topLidY = cy - currentHeight;
//             int bottomLidY = cy + currentHeight;

//             // MASKING: Erase everything outside the eyelids (draw black with color 0)
//             if (topLidY > 0) {
//                 m_gfx.fillRect(x, 0, 1, topLidY, 0); 
//             }
//             if (bottomLidY < ROWS) {
//                 m_gfx.fillRect(x, bottomLidY, 1, ROWS - bottomLidY, 0); 
//             }

//             // Outline the eye socket by drawing a white pixel exactly on the eyelid rim
//             m_gfx.fillRect(x, topLidY, 1, 1, 1);
//             m_gfx.fillRect(x, bottomLidY, 1, 1, 1);
//         }

//         // ==========================================
//         // 4. DRAW EYELASHES (When eye is mostly open)
//         // ==========================================
//         if (eyeOpenFactor > 0.7f) {
//             // Calculate current Y positions for the eyelashes along the curve
//             int lashY1 = cy - (int)(maxEyeHeight * (1.0f - (0.5f * 0.5f)) * eyeOpenFactor); 
//             int lashY2 = cy - (int)(maxEyeHeight * (1.0f - (0.8f * 0.8f)) * eyeOpenFactor); 

//             // Eyelashes closer to the center (Offset 15, ratio 0.32)
//             int lashY3 = cy - (int)(maxEyeHeight * (1.0f - (0.32f * 0.32f)) * eyeOpenFactor);

//             // Draw Left Eyelashes (fillRect: x, y, width, height, color)
//             m_gfx.fillRect(cx - 15, lashY3 - 5, 1, 5, 1); // New inner lash (taller)
//             m_gfx.fillRect(cx - 23, lashY1 - 4, 1, 4, 1);
//             m_gfx.fillRect(cx - 36, lashY2 - 3, 1, 3, 1); // Outer lash (shorter)

//             // Draw Right Eyelashes (mirrored)
//             m_gfx.fillRect(cx + 15, lashY3 - 5, 1, 5, 1); 
//             m_gfx.fillRect(cx + 23, lashY1 - 4, 1, 4, 1);
//             m_gfx.fillRect(cx + 36, lashY2 - 3, 1, 3, 1);
//         }

//         // Spin the Sharingan
//         rotation -= 0.15f; 
//         if (rotation <= -6.283f) rotation += 6.283f; 
//     }
// };

// #endif

// /*

// Bresenham’s Midpoint Circle Algorithm is a foundational computer graphics technique used to draw rasterized (pixelated) circles quickly and efficiently.
// Why It’s Perfect for Embedded Systems

//     Zero Floating-Point Math: It completely avoids heavy operations like sin(), cos(), or sqrt().

//     Integer-Only: It relies exclusively on fast integer addition, subtraction, and basic multiplication (which the C++ compiler automatically optimizes into bit-shifts).

//     High Framerates: Because microcontrollers like the ESP8266 lack dedicated graphics processing units, avoiding floating-point math prevents the CPU from bogging down during animations.

// How It Works: Two Core Concepts

// 1. Eight-Way Symmetry (Octants)
// A circle is perfectly symmetrical. You only need to mathematically calculate the pixels for a single 45-degree slice (1/8th of the circle).

// For every pixel you calculate in that first slice (x, y), you instantly know the coordinates for the other seven slices by flipping the signs and swapping X and Y:

//     (x, y) and (-x, y)

//     (x, -y) and (-x, -y)

//     (y, x) and (-y, x)

//     (y, -x) and (-y, -x)

// (This is exactly why your drawCircle code has 8 identical fillRect lines stacked together!)

// 2. The Midpoint Decision Parameter (p)
// When drawing a smooth curve on a rigid, square pixel grid, you can't place a pixel exactly on the true mathematical line. As the algorithm steps forward pixel-by-pixel along the X-axis, it has to choose between two possible pixels for the next step:

//     The pixel straight ahead.

//     The pixel straight ahead and down one level.

// The algorithm calculates the exact mathematical midpoint between those two pixels.

//     If the ideal circle passes above the midpoint, it keeps Y the same.

//     If the ideal circle passes below the midpoint, it steps Y down by 1.

// The decision variable p keeps track of this error margin continuously, updating itself using only integers as the loop progresses.

// */


/* ==============================================================================
 * ITACHI'S MANGEKYOU SHARINGAN - ANIMATION & GRAPHICS ENGINE
 * ==============================================================================
 *
 * --- PART 1: THE MATHEMATICS ---
 * 1. The Blades (Polar to Cartesian)
 *    3 blades, offset by 120 degrees (2.094 rad). Each blade is a curved "spine"
 *    sampled at N points. Along the spine, t goes 0 -> 1 (root -> tip):
 *      radius(t) = R_ROOT + t * (R_TIP - R_ROOT)
 *      angle(t)  = base_angle + t * BEND            (this bend makes it a scythe)
 *      width(t)  = W_MAX * sin(pi * (0.18 + 0.82t)) (fat near the root, needle tip)
 *    At each point we stamp a filled circle of that width, then convert polar
 *    back to pixels:  x = cx + r*cos(a),  y = cy + r*sin(a)
 *
 * 2. The Blinking Eye (Parabolas, Easing & Masking)
 *    The socket is an inverted parabola (1 - dx^2). Upper lid is taller than the
 *    lower lid, like a real eye. The blink value is smoothed with "smoothstep"
 *    (3t^2 - 2t^3) so lids accelerate/decelerate instead of moving linearly.
 *    Everything outside the lids is erased with black rectangles (masking).
 *
 * 3. Eyelashes (Quadratic Bezier Curves)
 *    Each lash grows from a point on the upper lid. Its shape is a Bezier:
 *      B(t) = (1-t)^2*P0 + 2(1-t)t*P1 + t^2*P2
 *    P0 = root on lid, P1 pulls the lash outward, P2 lifts the tip back up,
 *    so every lash sweeps out and curls up. Lashes fan outward toward the
 *    corner, get longer there, and droop down as the eyelid closes.
 *
 * 4. Bresenham's Midpoint Circle + Line (integer-only)
 *    Used for iris outline and for stitching the Bezier samples into pixels.
 *
 * --- PART 2: THE STACK EXECUTION (TOP-TO-BOTTOM) ---
 * 1. Service Layer (SharinganService): computes shapes, calls m_gfx.fillRect().
 * 2. Graphics Engine (Graphics): turns shapes into drawPixel(x, y, color) calls.
 * 3. Display Driver (SSD1306): maps (x, y) into its 1024-byte RAM buffer and
 *    sets/clears the bit with |= or &=.
 * 4. Hardware Abstraction (ESP8266I2C): update() slices the buffer into 16-byte
 *    chunks and sends them over I2C.
 *
 * --- PART 3: WHAT MAKES IT "ALIVE" ---
 *  - Eased blinks: fast close, slower open, random timing, occasional double blink
 *  - Spin surge: the Sharingan kicks faster after each blink, then settles
 *  - Gaze drift: the iris makes small darting saccades inside the socket
 *  - Pupil pulse: the inner ring "breathes"
 *  - Dense curved lashes on the upper lid + short lower lashes
 *  - Itachi's signature tear-trough lines under the eye
 * ============================================================================== */

#ifndef SHARINGAN_SERVICE_HPP
#define SHARINGAN_SERVICE_HPP

#include "utils/graphics/graphics.hpp"
#include <cmath>
#include <cstdint>

template <uint16_t ROWS, uint16_t COLS, uint16_t PAGES>
class SharinganService {
private:
    Graphics<ROWS, COLS, PAGES>& m_gfx;

    // ---------- Layout (tuned for 128x64) ----------
    enum : int {
        IRIS_R    = 22,   // outer iris radius
        EYE_W     = 50,   // half-width of the eye socket
        EYE_H_UP  = 23,   // max upper lid height above center
        EYE_H_LOW = 17,   // max lower lid height below center
        R_ROOT    = 7,    // blade root radius
        R_TIP     = 19,   // blade tip radius
        BLADE_PTS = 16    // samples per blade (higher = smoother)
    };
    static constexpr bool ITACHI_LINES = true; // tear-trough lines under the eye

    // ---------- Animation state ----------
    float rotation  = 0.0f;
    float surge     = 0.0f;   // extra spin speed, decays back to 0
    float pulse     = 0.0f;   // pupil breathing phase

    float gazeX = 0, gazeY = 0, targetX = 0, targetY = 0;
    int   gazeTimer = 40;

    enum BlinkState { IDLE, CLOSING, OPENING };
    BlinkState blinkState = IDLE;
    float blinkT = 1.0f;          // 1 = open, 0 = closed (linear)
    float eyeOpenFactor = 1.0f;   // eased version of blinkT
    int   blinkTimer = 0;
    int   nextBlinkAt = 100;
    bool  justDoubled = false;

    uint32_t rng = 0xA5C3E1u;

    // ---------- Helpers ----------
    int rnd(int lo, int hi) {   // xorshift, inclusive range
        rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;
        return lo + (int)(rng % (uint32_t)(hi - lo + 1));
    }

    void plot(int x, int y, int c = 1) {
        if (x >= 0 && x < COLS && y >= 0 && y < ROWS) m_gfx.fillRect(x, y, 1, 1, c);
    }

    // Bresenham line
    void drawLine(int x0, int y0, int x1, int y1) {
        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        while (true) {
            plot(x0, y0);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    // Filled circle drawn as horizontal spans (much faster than per-pixel)
    void fillCircle(int cx, int cy, int r, int c = 1) {
        for (int dy = -r; dy <= r; dy++) {
            int y = cy + dy;
            if (y < 0 || y >= ROWS) continue;
            int dx = (int)sqrtf((float)(r * r - dy * dy));
            int x0 = cx - dx, x1 = cx + dx;
            if (x0 < 0) x0 = 0;
            if (x1 >= COLS) x1 = COLS - 1;
            if (x1 >= x0) m_gfx.fillRect(x0, y, x1 - x0 + 1, 1, c);
        }
    }

    // Bresenham midpoint circle outline
    void drawCircle(int cx, int cy, int r) {
        int x = 0, y = r, p = 3 - 2 * r;
        while (y >= x) {
            plot(cx + x, cy + y); plot(cx - x, cy + y);
            plot(cx + x, cy - y); plot(cx - x, cy - y);
            plot(cx + y, cy + x); plot(cx - y, cy + x);
            plot(cx + y, cy - x); plot(cx - y, cy - x);
            x++;
            if (p > 0) { y--; p += 4 * (x - y) + 10; }
            else       { p += 4 * x + 6; }
        }
    }

    // Curved lash: quadratic Bezier sampled into short line segments
    void drawBezier(float x0, float y0, float x1, float y1, float x2, float y2, int segs) {
        int px = (int)lroundf(x0), py = (int)lroundf(y0);
        for (int i = 1; i <= segs; i++) {
            float t = (float)i / segs, u = 1.0f - t;
            int nx = (int)lroundf(u * u * x0 + 2 * u * t * x1 + t * t * x2);
            int ny = (int)lroundf(u * u * y0 + 2 * u * t * y1 + t * t * y2);
            drawLine(px, py, nx, ny);
            px = nx; py = ny;
        }
    }

    // Lid heights at a given x (0..1 parabola * blink)
    float curveAt(int x, int cx) const {
        float dx = (float)abs(x - cx) / EYE_W;
        return 1.0f - dx * dx;
    }

    // ---------- State updates ----------
    void updateBlink() {
        switch (blinkState) {
            case IDLE:
                if (++blinkTimer >= nextBlinkAt) { blinkState = CLOSING; blinkTimer = 0; }
                break;
            case CLOSING:
                blinkT -= 0.24f;                     // snap shut
                if (blinkT <= 0.0f) { blinkT = 0.0f; blinkState = OPENING; }
                break;
            case OPENING:
                blinkT += 0.12f;                     // ease back open
                if (blinkT >= 1.0f) {
                    blinkT = 1.0f; blinkState = IDLE;
                    surge = 0.35f;                   // spin kick after each blink
                    if (!justDoubled && rnd(0, 4) == 0) { nextBlinkAt = 5;  justDoubled = true; }
                    else                                { nextBlinkAt = rnd(90, 230); justDoubled = false; }
                }
                break;
        }
        eyeOpenFactor = blinkT * blinkT * (3.0f - 2.0f * blinkT); // smoothstep
    }

    void updateGaze() {
        if (--gazeTimer <= 0) {
            if (rnd(0, 2) == 0) { targetX = 0; targetY = 0; }   // look back at you
            else { targetX = (float)rnd(-4, 4); targetY = (float)rnd(-2, 2); }
            gazeTimer = rnd(30, 110);
        }
        gazeX += (targetX - gazeX) * 0.28f;  // quick saccade
        gazeY += (targetY - gazeY) * 0.28f;
    }

    // ---------- Drawing ----------
    void drawIris(int cx, int cy) {
        drawCircle(cx, cy, IRIS_R);
        drawCircle(cx, cy, IRIS_R - 1);

        // Three curved blades with tapered, needle-sharp tips
        for (int b = 0; b < 3; b++) {
            float base = rotation + b * 2.09439f;
            for (int i = 0; i < BLADE_PTS; i++) {
                float t = (float)i / (BLADE_PTS - 1);
                float r = R_ROOT + t * (R_TIP - R_ROOT);
                float a = base + t * 1.15f;                       // scythe bend
                float w = 4.6f * sinf(3.14159f * (0.18f + 0.82f * t));
                int px = cx + (int)lroundf(r * cosf(a));
                int py = cy + (int)lroundf(r * sinf(a));
                fillCircle(px, py, (int)(w + 0.5f));
            }
        }

        // Pupil ring that breathes, with a black hollow center
        int ringR = 6 + (int)(1.6f * (sinf(pulse) + 1.0f) * 0.5f);
        fillCircle(cx, cy, ringR, 1);
        fillCircle(cx, cy, ringR - 2, 0);
    }

    void drawUpperLashes(int cx, int cy) {
        float droop = 1.0f - eyeOpenFactor;
        float up = 1.0f - 1.6f * droop;               // >0 lashes point up, <0 they sag down
        float lenScale = 0.45f + 0.55f * eyeOpenFactor;

        for (int x = cx - EYE_W + 3; x <= cx + EYE_W - 3; x += 2) {
            float a = (float)abs(x - cx) / EYE_W;               // 0 center .. 1 corner
            float side = (x < cx) ? -1.0f : 1.0f;
            int ly = cy - (int)(EYE_H_UP * curveAt(x, cx) * eyeOpenFactor);

            // Fixed per-lash variation (no flicker): pattern of long/medium/short
            int variant = ((x * 5) % 3);                          // 0..2
            float len = (4.0f + 5.5f * a + variant * 1.5f) * lenScale;
            float lean = side * (0.20f + 1.05f * a);              // fan outward

            float x1 = x + lean * len * 0.55f, y1 = ly - len * 0.45f * up;
            float x2 = x + lean * len * 0.85f, y2 = ly - len * 1.00f * up;
            drawBezier((float)x, (float)ly, x1, y1, x2, y2, len > 7 ? 5 : 3);
        }
    }

    void drawLowerLashes(int cx, int cy) {
        if (eyeOpenFactor < 0.3f) return;
        for (int x = cx - EYE_W + 9; x <= cx + EYE_W - 9; x += 5) {
            float a = (float)abs(x - cx) / EYE_W;
            float side = (x < cx) ? -1.0f : 1.0f;
            int ly = cy + (int)(EYE_H_LOW * curveAt(x, cx) * eyeOpenFactor);
            int len = 2 + ((x * 3) % 2);
            drawLine(x, ly + 1, x + (int)(side * a * 2.0f), ly + 1 + len);
        }
    }

    void drawTearTroughs(int cx, int cy) {
        // Itachi's lines: two arcs following the lower lid, mirrored left/right
        for (int x = cx - 36; x <= cx + 36; x++) {
            int ax = abs(x - cx);
            float base = EYE_H_LOW * curveAt(x, cx);
            if (ax >= 12 && ax <= 34) plot(x, cy + (int)base + 8);   // long line
            if (ax >= 18 && ax <= 30) plot(x, cy + (int)base + 12);  // short line
        }
    }

public:
    SharinganService(Graphics<ROWS, COLS, PAGES>& gfx) : m_gfx(gfx) {}

    void nextFrame() {
        m_gfx.clear();

        const int cx = COLS / 2;
        const int cy = ROWS / 2;

        updateBlink();
        updateGaze();

        // 1. Iris follows the gaze; lids stay fixed on the face
        drawIris(cx + (int)lroundf(gazeX), cy + (int)lroundf(gazeY));

        // 2. Eyelid masking + rims
        for (int x = cx - EYE_W; x <= cx + EYE_W; x++) {
            float curve = curveAt(x, cx);
            int topY = cy - (int)(EYE_H_UP  * curve * eyeOpenFactor);
            int botY = cy + (int)(EYE_H_LOW * curve * eyeOpenFactor);

            if (topY > 0)    m_gfx.fillRect(x, 0, 1, topY, 0);
            if (botY < ROWS) m_gfx.fillRect(x, botY, 1, ROWS - botY, 0);

            plot(x, topY);
            plot(x, topY - 1);   // heavier upper lid line
            plot(x, botY);
        }

        // 3. Lashes + face lines (drawn after masking so they sit outside the eye)
        drawUpperLashes(cx, cy);
        drawLowerLashes(cx, cy);
        if (ITACHI_LINES) drawTearTroughs(cx, cy);

        // 4. Advance spin / pulse
        rotation -= 0.10f + surge;
        if (rotation <= -6.2832f) rotation += 6.2832f;
        surge *= 0.94f;
        pulse += 0.12f;
        if (pulse > 6.2832f) pulse -= 6.2832f;
    }
};

#endif