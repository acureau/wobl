#include <vector>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include "Oscillator.hpp"
#include "Filter.hpp"
#include "Noise.hpp"
#include "Wav.hpp"
#include "Envelope.hpp"

int main()
{
    // Initialize RNG.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::srand(seed);

    // Generate waveform.
    int duration_seconds = 5;
    int sample_rate = 44100;
    int channel_count = 2;
    int sample_count = sample_rate * duration_seconds; // Samples per channel
    std::vector<float> samples(sample_count * channel_count); // Interleaved buffer

    // Create oscillators & envelope
    float frequency = 123.75f;
    Oscillator saw_voice(OscillatorType::Sawtooth, frequency, static_cast<float>(sample_rate));
    Oscillator sine_voice(OscillatorType::Sine, frequency, static_cast<float>(sample_rate));
    ADSR envelope(static_cast<float>(sample_rate), 0.05f, 2.0f, 0.0f, 0.0f);
    SinglePole filter(FilterType::LowPass, 0.75);

    // Generate interleaved samples
    envelope.Trigger();
    for (int i = 0; i < sample_count; i++)
    {
        if (i == sample_rate * 0.15)
        {
            envelope.Trigger();
        }

        if (i == sample_rate * 0.30)
        {
            envelope.Trigger();
        }

        if (i == sample_rate * 0.45)
        {
            envelope.Trigger();
        }

        if (i == sample_rate * 0.60)
        {
            frequency = 123.75f + 0.2 * 123.75f;
            envelope.Trigger();
        }

        if (i == sample_rate * 0.75)
        {
            frequency = 123.75f + 0.4 * 123.75f;
            envelope.Trigger();
        }

        if (i == sample_rate * 1.0)
        {
            frequency = 123.75f + 0.6 * 123.75f;
            envelope.Trigger();
        }

        if (i == sample_rate * 1.30)
        {
            frequency = 123.75f + 0.4 * 123.75f;
            envelope.Trigger();
        }

        if (i == sample_rate * 1.45)
        {
            frequency = 123.75f + 0.3 * 123.75f;
            envelope.Trigger();
        }

        if (i == sample_rate * 1.60)
        {
            frequency = 123.75f;
            envelope.Trigger();
        }

        if (i == sample_rate * 1.75)
        {
            frequency = 123.75f + 0.2 * 123.75f;
            envelope.Trigger();
        }

        if (i == sample_rate * 1.90)
        {
            frequency = 123.75f;
            envelope.Trigger();
        }

        float envelope_level = envelope.GetLevel();
        saw_voice.SetFrequency(envelope_level * frequency);
        sine_voice.SetFrequency(envelope_level * frequency);
        float sample_value = envelope_level * ((0.3f * filter.Filter(saw_voice.Sample())) + (0.7f * sine_voice.Sample()));

        // Interleave samples into buffer
        for (int ch = 0; ch < channel_count; ch++)
        {
            samples[i * channel_count + ch] = sample_value;
        }
    }

    // Export to WAV.
    export_wav("pew.wav", channel_count, sample_rate, samples);
    return 0;
}