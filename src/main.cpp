#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <memory>
#include "oscillators.hpp"
#include "wav.hpp"

int main()
{
    // Initialize RNG.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::srand(seed);

    // Generate waveform.
    constexpr int32_t sample_rate = 44100;
    constexpr int duration_seconds = 20;
    const int sample_count = sample_rate * duration_seconds;

    // Use std::vector instead of raw memory allocation.
    std::vector<float> samples(sample_count);

    for (int i = 0; i < sample_count; i++)
    {
        float time = static_cast<float>(i) / sample_rate;
        // float modulator = sine(2.0f, time, 0.0f);
        samples[i] = sine(440.0f, time, 0.0f); // 10.0f * modulator
    }

    // Export to WAV.
    export_wav("sine.wav", 1, sample_rate, samples);
    return 0;
}
