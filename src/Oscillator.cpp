#include <cmath>
#include <cfloat>
#include <numbers>
#include <cstdlib>
#include "Oscillator.hpp"

void Oscillator::SetFrequency(float frequency)
{
    this->Frequency = frequency;
    this->PhaseIncrement = frequency / this->SampleRate;
}

void Oscillator::SetSampleRate(float sample_rate)
{
    this->SampleRate = sample_rate;
    this->PhaseIncrement = this->Frequency / sample_rate;
}

void Oscillator::Configure(
    std::optional<float> frequency,
    std::optional<float> sample_rate,
    std::optional<OscillatorType> type,
    std::optional<float> phase,
    std::optional<float> phase_offset,
    std::optional<float> speed
)
{
    this->Frequency = frequency.value_or(this->Frequency);
    this->SampleRate = sample_rate.value_or(this->SampleRate);
    this->Type = type.value_or(this->Type);
    this->Phase = phase.value_or(this->Phase);
    this->PhaseOffset = phase_offset.value_or(this->PhaseOffset);
    this->Speed = speed.value_or(this->Speed);

    if (frequency || sample_rate)
    {
        this->PhaseIncrement = this->Frequency / this->SampleRate;
    }
}

float Oscillator::Sample()
{
    // Wrap phase.
    if (this->Phase < 0.0f || this->Phase >= 1.0f)
    {
        this->Phase = fmodf(this->Phase, 1.0f);
        if (this->Phase < 0.0f) 
        {
            this->Phase += 1.0f;
        }
    }

    // Calculate sample.
    float sample = 0.0f;
    if (fabsf(this->Frequency) >= FLT_EPSILON)
    {
        switch (this->Type)
        {
            case OscillatorType::Sine:
                sample = sinf(2.0f * std::numbers::pi * (this->Phase + this->PhaseOffset));
                break;

            case OscillatorType::Square:
                sample = (fmodf(this->Phase + this->PhaseOffset, 1.0f) < 0.5f) ? 1.0f : -1.0f;
                break;

            case OscillatorType::Sawtooth:
                sample = fmodf(this->Phase + this->PhaseOffset, 1.0f);
                if (sample < 0.0f)
                {
                    sample += 1.0f;
                }
                sample = 2.0f * sample - 1.0f;
                break;

            case OscillatorType::Triangle:
                sample = fmodf(2.0f * (this->Phase + this->PhaseOffset), 2.0f);
                if (sample < 0.0f)
                {
                    sample += 2.0f;
                }
                sample = 2.0f * (1.0f - fabsf(sample - 1.0f)) - 1.0f;
                break;
        }
    }

    // Increment phase and return.
    this->Phase += this->PhaseIncrement * this->Speed;
    return sample;
}

Oscillator::Oscillator(OscillatorType type, float frequency, float sample_rate): 
    Type(type),
    Frequency(frequency),
    SampleRate(sample_rate),
    PhaseIncrement(frequency / sample_rate) 
{}