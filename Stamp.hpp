/*
Author: Finley Huggins
Date: February 14, 2025
*/

#ifndef __STAMP_HPP__
#define __STAMP_HPP__

#include <atomic>

enum StampVariety {
    Ellipse
};

class Stamp
{
public:
    // Rotations are in radians
    Stamp(StampVariety variety); // Default to 64 x 64
    Stamp(StampVariety variety, size_t w, size_t h);
    ~Stamp();
    static void integrateImpulse(
        // Information for the stamp.
        // Width, height, centerX, and centerY are normalized,
        // though the exact constraints on their values is complicated;
        // [centerX - width/2.0, centerX + width/2.0] must have nonempty intersection with [0,1],
        // and likewise for the vertical axis.
        // (In other words, the stamp must be placed at least partially on screen.)
        const Stamp& stamp, double width, double height, double centerX, double centerY, double maxStrength,
        // Information for the texels
        std::uint8_t* texels, size_t w, size_t h, bool verticalPass
    );
private:
    size_t w, h;
    float rotation;
    std::int8_t* stamp;

    void setStamp(StampVariety variety);
};

#endif
