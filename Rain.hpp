/*
Author: Finley Huggins
Date Modified: April 4, 2025
*/


#ifndef __RAIN_HPP__
#define __RAIN_HPP__

#include <random>
#include "TexDyn.hpp"

class RainCreator
{
public:
    RainCreator();
    void addRaindrop(DynamicTexture &texture);
    void timeStep(size_t milliseconds, DynamicTexture &texture);
    void multiplyRate(float rateMultiplier);

private:
    size_t countdown;
    float rate;
    float rateVariation;
    float minRainRadius;
    float maxRainRadius;
    std::random_device rd;
    std::mt19937 generator;
    std::uniform_real_distribution<> dist;
};

#endif
