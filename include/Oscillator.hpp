#pragma once

#include <optional>

enum class OscillatorType
{
    Sine,
    Square,
    Sawtooth,
    Triangle
};

class Oscillator
{
    private:
        float Frequency;            // Cycles per second.
        float SampleRate;           // Samples per second.
        float PhaseIncrement;       // Phase increment per sample.

    public:
        OscillatorType Type;        // The type of oscillator.
        float Phase = 0.0f;         // Current phase.
        float PhaseOffset = 0.0f;   // Phase offset.
        float Speed = 1.0f;         // Oscillator speed multiplier.

        // Sets the frequency, aligning phase recalculating the phase increment.
        void SetFrequency(float frequency);

        // Sets the sample rate, recalculating the phase increment.
        void SetSampleRate(float sample_rate);

        // Configures any oscillator property in a single method call. 
        // (Prefer direct assignment and setter functions for real-time applications.)
        void Configure(
            std::optional<float> frequency = std::nullopt,
            std::optional<float> sample_rate = std::nullopt,
            std::optional<OscillatorType> type = std::nullopt,
            std::optional<float> phase = std::nullopt,
            std::optional<float> phase_offset = std::nullopt,
            std::optional<float> speed = std::nullopt
        );

        // Samples the oscillator and increments the phase.
        float Sample();

        // Creates an oscillator with the given type, frequency, and sample rate.
        Oscillator(OscillatorType type, float frequency, float sample_rate);
};