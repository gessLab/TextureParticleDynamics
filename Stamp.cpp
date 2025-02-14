/*
Author: Finley Huggins
Date: February 14, 2025
*/
#include <iostream>
#include <fstream>
#include <cmath>
#include "Stamp.hpp"
#include "ThreadPool.hpp"
#include "TexDyn.hpp"

// Plain Old Data structs for passing into ThreadPool::dispatch
struct InitData {
    size_t w;
    size_t h;
    std::int8_t* stamp;
};

struct ImpulseData {
    // Dimensions of the part of the stamp actually drawn from
    size_t s_start_row, s_start_col, s_height, s_width;
    // Dimensions of the part of the texels actually drawn to
    size_t t_start_row, t_start_col, t_height, t_width;
    // Width of the *entire* stamp or texels, not just the part drawn to/from
    size_t stamp_w, texels_w;
    std::int8_t* stamp;
    std::uint8_t* texels;
    double maxStrength;
};

// ===========================================================================
//      Initialization (and destructor)
// ===========================================================================

void initEllipse(std::mutex& m, size_t begin, size_t end, void* data);

Stamp::Stamp(StampVariety variety)
{
    w = 64;
    h = 64;
    stamp = new std::int8_t[2 * w * h];
    setStamp(variety);
}

Stamp::Stamp(StampVariety variety, size_t w, size_t h)
{
    this->w = w;
    this->h = h;
    stamp = new std::int8_t[2 * w * h];
    setStamp(variety);
}

Stamp::~Stamp()
{
    if (stamp != nullptr) delete[] stamp;
}

void Stamp::setStamp(StampVariety variety)
{
    InitData initData = InitData
    {
        .w = w,
        .h = h,
        .stamp = stamp
    };

    switch (variety)
    {
        default:
            std::cerr << "Provided invalid StampVariety, defaulting to Ellipse\n";
        case StampVariety::Ellipse:
            ThreadsPool::pool().dispatch(w * h, &(initEllipse), static_cast<void*>(&initData), true);
    }
}

void initEllipse(std::mutex& m, size_t begin, size_t end, void* data) {
    InitData* initData = static_cast<InitData*>(data);
    size_t w = initData->w;
    size_t h = initData->h;
    std::int8_t* ref = initData->stamp;
    // Limitation: rotation is currently ignored

    double cx = (double) (w-1) / 2.;
    double cy = (double) (h-1) / 2.;

    //NOTE: preemptively subtracting 1 as we start the for-loop by incrementing
    size_t row = begin / w, col = (begin % w) - 1;
    size_t index = begin * 2;
	for (size_t i = begin; i < end; ++i, index += 2)
	{
		if (++col == w)
		{
			col = 0;
			row += 1;
		}

        // dx and dy, normalized to unit circle
        double dx = (((double) col) - cx) * 2.0 / (double) w;
        double dy = (((double) row) - cy) * 2.0 / (double) h;

        double rSquared = dx*dx + dy*dy;

        if (rSquared > 1.0) continue;

        // The max int8_t value is 127. We use [-127, 127]
        double impulseStrength = 127. * (std::sqrt(2 - rSquared) - 1) / (std::sqrt(2) - 1);

        int8_t xImpulse = (int8_t) std::round(impulseStrength * dx / std::sqrt(rSquared));
        int8_t yImpulse = (int8_t) std::round(impulseStrength * dy / std::sqrt(rSquared));

        ref[index] = xImpulse;
        ref[index + 1] = yImpulse;
	}
}

// ===========================================================================
//      Integration
// ===========================================================================

// [center - scale/2.0, center + scale/2.0] should intersect [0,1]
// If [center - scale/2.0, center + scale/2.0] is normalized to [0,1], then
// [normStart, normEnd] gives the intersection with [0,1]
double normStart(double scale, double center)
{
    double attemptedStart = center - scale / 2.0;
    double attemptedEnd = center + scale / 2.0;

    if (attemptedStart >= 0. || attemptedEnd == attemptedStart)
    {
        return 0.;
    }
    else
    {
        return (0. - attemptedStart) / (attemptedEnd - attemptedStart);
    }
}

// [center - scale/2.0, center + scale/2.0] should intersect [0,1]
// If [center - scale/2.0, center + scale/2.0] is normalized to [0,1], then
// [normStart, normEnd] gives the intersection with [0,1]
double normEnd(double scale, double center)
{
    double attemptedStart = center - scale / 2.0;
    double attemptedEnd = center + scale / 2.0;

    if (attemptedEnd <= 1. || attemptedEnd == attemptedStart)
    {
        return 1.;
    }
    else
    {
        return (1. - attemptedStart) / (attemptedEnd - attemptedStart);
    }
}

double lerp(double val, double start, double end) {
    return start * (1. - val) + end * val;
}

void impulseIntegrate(std::mutex& m, size_t begin, size_t end, void* texels)
{
	// Mostly copied from TexDyn's integrate
	std::uint8_t* ref = static_cast<std::uint8_t*>(texels);

	size_t index = begin * 4;
	for (size_t i = begin; i < end; ++i, index += 4)
	{
		//Integrate new texel particle stack size
		//Blue channel is subtraction, green is addition
		//Alpha stores this change
		ref[index + 3] -= ref[index + 2];
		ref[index + 3] += ref[index + 1];
		ref[index + 2] = 0;
		ref[index + 1] = 0;
	}
}

void impulseHorizontal(std::mutex& m, size_t begin, size_t end, void* data)
{
    ImpulseData impulse = *static_cast<ImpulseData*>(data);

    for (size_t y = 0; y < impulse.t_height; y++)
    {
        for (size_t x = 0; x < impulse.t_width; x++)
        {
            size_t sourceY = (y * impulse.s_height) / impulse.t_height + impulse.s_start_row;
            size_t sourceX = (x * impulse.s_width ) / impulse.t_width  + impulse.s_start_col;
            size_t i = (sourceY * impulse.stamp_w + sourceX);

            int8_t xImpulse = impulse.stamp[ 2*i     ];
            // int8_t yImpulse = impulse.stamp[ 2*i + 1 ];

            size_t t_i = (y + impulse.t_start_row) * impulse.texels_w + (x + impulse.t_start_col);

            if (xImpulse < 0)
            {
                impulse.texels[4 * t_i] = 1;
            }
            else if (xImpulse > 0)
            {
                impulse.texels[4 * t_i] = 2;
            }
        }
    }
}

// Handle y impulse
void impulseVertical(std::mutex& m, size_t begin, size_t end, void* data)
{
    ImpulseData impulse = *static_cast<ImpulseData*>(data);

    for (size_t y = 0; y < impulse.t_height; y++)
    {
        for (size_t x = 0; x < impulse.t_width; x++)
        {
            size_t sourceY = (y * impulse.s_height) / impulse.t_height + impulse.s_start_row;
            size_t sourceX = (x * impulse.s_width ) / impulse.t_width  + impulse.s_start_col;
            size_t i = (sourceY * impulse.stamp_w + sourceX);

            // int8_t xImpulse = impulse.stamp[ 2*i     ];
            int8_t yImpulse = impulse.stamp[ 2*i + 1 ];

            size_t t_i = (y + impulse.t_start_row) * impulse.texels_w + (x + impulse.t_start_col);

            if (yImpulse < 0)
            {
                impulse.texels[4 * t_i] = 1;
            }
            else if (yImpulse > 0)
            {
                impulse.texels[4 * t_i] = 2;
            }
        }
    }
}

// width, height, centerX, and centerY should be normalized, see header for more
void Stamp::integrateImpulse(
    const Stamp& stamp, double width, double height, double centerX, double centerY, double maxStrength,
    std::uint8_t* texels, size_t w, size_t h, bool verticalPass
) {

    // Thes 8 numbers are all normalized (as multiples of stamp/texel width or height)
    double stampStartY = normStart(height, centerY);
    double stampEndY   = normEnd  (height, centerY);
    double stampStartX = normStart(width,  centerX);
    double stampEndX   = normEnd  (width,  centerX);

    double texStartY = std::max(0., centerY - height/2.);
    double texEndY   = std::min(1., centerY + height/2.);
    double texStartX = std::max(0., centerX -  width/2.);
    double texEndX   = std::min(1., centerX +  width/2.);

    ImpulseData impulseData = ImpulseData
    {
        .s_start_row  = (size_t) (stampStartY * (double) stamp.h),
        .s_start_col  = (size_t) (stampStartX * (double) stamp.w),
        .s_height     = (size_t) ((stampEndY - stampStartY) * (double) stamp.h),
        .s_width      = (size_t) ((stampEndX - stampStartX) * (double) stamp.w),

        .t_start_row  = (size_t) (texStartY * (double) h),
        .t_start_col  = (size_t) (texStartX * (double) w),
        .t_height     = (size_t) ((texEndY - texStartY) * (double) h),
        .t_width      = (size_t) ((texEndX - texStartX) * (double) w),

        .stamp_w      = stamp.h,
        .texels_w     = w,
        .stamp        = stamp.stamp,
        .texels       = texels,
        .maxStrength  = maxStrength
    };

    if (!verticalPass)
    {
        ThreadsPool::pool().dispatch(w * h, &(impulseIntegrate),  static_cast<void*>(texels),       true);
        ThreadsPool::pool().dispatch(w * h, &(impulseHorizontal), static_cast<void*>(&impulseData), true);
        ThreadsPool::pool().dispatch(w * h, &(kernelHorizontal),  static_cast<void*>(texels),       true);

        ThreadsPool::pool().dispatch(w * h, &(impulseIntegrate),  static_cast<void*>(texels),       true);
        ThreadsPool::pool().dispatch(w * h, &(impulseVertical),   static_cast<void*>(&impulseData), true);
        ThreadsPool::pool().dispatch(w * h, &(kernelVertical),    static_cast<void*>(texels),       true);
    }
    else
    {
        ThreadsPool::pool().dispatch(w * h, &(impulseIntegrate),  static_cast<void*>(texels),        true);
        ThreadsPool::pool().dispatch(w * h, &(impulseVertical),   static_cast<void*>(&impulseData), true);
        ThreadsPool::pool().dispatch(w * h, &(kernelVertical),    static_cast<void*>(texels),        true);

        ThreadsPool::pool().dispatch(w * h, &(impulseIntegrate),  static_cast<void*>(texels),       true);
        ThreadsPool::pool().dispatch(w * h, &(impulseHorizontal), static_cast<void*>(&impulseData), true);
        ThreadsPool::pool().dispatch(w * h, &(kernelHorizontal),  static_cast<void*>(texels),       true);
    }
}
