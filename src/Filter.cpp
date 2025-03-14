#include <cmath>
#include <optional>
#include "Filter.hpp"

void SinglePole::SetSmoothingFactor(float smoothing_factor)
{
    smoothing_factor = std::max(0.0f, std::min(1.0f, smoothing_factor));
    SmoothingFactor = smoothing_factor;
}

float SinglePole::Filter(float input)
{
    // Set previous input for the first time. Do not apply filter.
    if (!this->PreviousOutput || !this->PreviousInput)
    {
        this->PreviousOutput = input;
        this->PreviousInput = input;
        return input;
    }
    
    // Calculate filtered input. 
    float output = input;
    switch (this->Type)
    {
        case FilterType::LowPass:
            output = this->SmoothingFactor * input + (1.0f - this->SmoothingFactor) * *this->PreviousOutput;
            break;

        case FilterType::HighPass:
            output = (1.0f - this->SmoothingFactor) * (*this->PreviousOutput + input - *this->PreviousInput);
            break;
    }

    // Set last input, return filtered input.
    this->PreviousInput = input;
    this->PreviousOutput = output;
    return output;
}

SinglePole::SinglePole(FilterType type, float smoothing_factor): Type(type) 
{
    this->SetSmoothingFactor(smoothing_factor);
}