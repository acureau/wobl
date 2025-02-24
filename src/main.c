#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "oscillators.h"

int main()
{
    // Initialize RNG.
    srand(time(NULL));

    for (float time = -10.0f; time <= 10.0f; time += 0.001f)
    {
        printf("%f\n", noise());
    }
    return 0;
}