/*
Author: Finley Huggins
Date Modified: April 4, 2025
*/

#include "Rain.hpp"

RainCreator::RainCreator(): countdown(0), rate(0.01), rateVariation(0.5),
                            minRainRadius(0.01), maxRainRadius(0.03)
{
    generator = std::mt19937(rd());
    dist = std::uniform_real_distribution<>(0., 1.);
}

void RainCreator::addRaindrop(DynamicTexture& texture)
{
    float x = dist(generator);
    float y = dist(generator);

    float radiusVariation = dist(generator);
    float radius = minRainRadius + (maxRainRadius - minRainRadius) * radiusVariation;

    texture.addRaindrop(x, y, radius);
}


void RainCreator::timeStep(size_t milliseconds, DynamicTexture& texture)
{
    if (countdown <= milliseconds)
    {
        float variation = dist(generator) * 2. - 1.;
        float chosenRate = rate * (1. + rateVariation * variation);

        countdown = static_cast<size_t>(1. / chosenRate);

        addRaindrop(texture);
    }
    else
    {
        countdown -= milliseconds;
    }

}

void RainCreator::multiplyRate(float rateMultiplier)
{
    rate *= rateMultiplier;
}
