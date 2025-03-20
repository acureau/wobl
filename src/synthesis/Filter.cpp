#include <cmath>
#include <optional>
#include "Filter.hpp"

void SinglePole::SetSmoothingFactor(float smoothing_factor)
{
    smoothing_factor = std::max(0.0f, std::min(1.0f, smoothing_factor));
    SmoothingFactor = smoothing_factor;
}

float SinglePole::Filter(float current)
{
    // Set previous for the first time. Do not apply filter.
    if (!this->Previous)
    {
        this->Previous = current;
        return current;
    }

    // Calculate low-pass, update previous.
    float low_pass_output = this->SmoothingFactor * current + (1.0f - this->SmoothingFactor) * *this->Previous;
    this->Previous = low_pass_output;

    // Calculate high-pass.
    if (this->Type == FilterType::HighPass)
    {
        return current - low_pass_output;
    }

    // Return low-pass.
    else if (this->Type == FilterType::LowPass)
    {
        return low_pass_output;
    }

    // Don't filter.
    return current;
}

SinglePole::SinglePole(FilterType type, float smoothing_factor): Type(type) 
{
    this->SetSmoothingFactor(smoothing_factor);
}