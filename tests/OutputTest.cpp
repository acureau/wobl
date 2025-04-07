#include <chrono>
#include <iostream>
#include "Oscillator.hpp"
#include "Wav.hpp"

int main()
{
    // Initialize RNG.
    // todo: move into library.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::srand(seed);

    // Generate waveform.
    int duration_seconds = 5;
    int sample_rate = 44100;
    int channel_count = 1;
    int sample_count = sample_rate * duration_seconds;
    std::vector<float> samples(sample_count * channel_count);

    // Create sine oscillilator.
    float frequency = 440.0f;
    Oscillator sine(OscillatorType::Sine, frequency, static_cast<float>(sample_rate));

    // todo: configure live output.
    

    // Generate samples.
    // todo: produce samples to live output.
    for (int i = 0; i < sample_count; i++)
    {
        samples[i] = sine.Sample();
    }

    // Export to WAV.
    export_wav("sine.wav", channel_count, sample_rate, samples);
    return 0;
}