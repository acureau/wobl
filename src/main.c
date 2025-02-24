#include <stdio.h>
#include "oscillators.h"

int main()
{
    for (float time = -10.0f; time <= 10.0f; time += 0.001f)
    {
        printf("%f\n", triangle(-1.0f, time, 0.0f));
    }
    return 0;
}