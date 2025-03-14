#pragma once

#include <cmath>
#include <optional>

enum FilterType
{
    LowPass,
    HighPass
};

class SinglePole
{
    private:
        std::optional<float> PreviousOutput;    // The previous input sample.
        std::optional<float> PreviousInput;     // The previous output sample.
        float SmoothingFactor = 0.0f;           // A smoothing factor between 0 and 1.

    public:
        FilterType Type;                    // The filtering type.

        // Sets the smoothing factor of the filter, enforces a 0 to 1 range.
        void SetSmoothingFactor(float smoothing_factor);

        // Calculates the filtered sample value.
        float Filter(float input);
        
        // Creates a single pole filter.
        SinglePole(FilterType type, float smoothing_factor);
};