#include <cstdlib>
#include "Noise.hpp"

float Noise()
{
    return 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;
}