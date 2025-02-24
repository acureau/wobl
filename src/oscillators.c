#define _USE_MATH_DEFINES

#include <math.h>
#include <float.h>
#include "oscillators.h"

/*
    Notes:
    The square, sawtooth, and triangle functions need to adjust their bounds to match sine. When frequency is zero we don't perform any adjustment.
    The fmodf function maintains the sign of the first argument, meaning we need to account for negative return values.
*/

float sine(float frequency, float time, float phase_shift)
{
    return sinf(2 * M_PI * (frequency * time + phase_shift));
}

float square(float frequency, float time, float phase_shift)
{
    if (fabsf(frequency) < FLT_EPSILON)
    {
        return 0.0f;
    }

    return 2 * fabsf(fmodf(floorf(2.0f * (frequency * time + phase_shift)), 2.0f)) - 1;
}

float sawtooth(float frequency, float time, float phase_shift)
{
    if (fabsf(frequency) < FLT_EPSILON)
    {
        return 0.0f;
    }

    float amplitude = fmodf(frequency * time + phase_shift, 1.0f);
    if (amplitude < 0.0f)
    {
        amplitude += 1.0f;
    }
    return 2.0f * amplitude - 1.0f;
}

float triangle(float frequency, float time, float phase_shift)
{
    if (fabsf(frequency) < FLT_EPSILON)
    {
        return 0.0f;
    }

    float amplitude = fmodf(2.0f * (frequency * time + phase_shift), 2.0f);
    if (amplitude < 0.0f)
    {
        amplitude += 2.0f;
    }
    amplitude = fabsf(amplitude - 1.0f);
    return 2.0f * amplitude - 1.0f;
}