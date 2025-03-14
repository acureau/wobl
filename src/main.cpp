#include <vector>
#include <cstdlib>
#include <chrono>
#include "Oscillator.hpp"
#include "Filter.hpp"
#include "Noise.hpp"
#include "Wav.hpp"

int main()
{
    // Initialize RNG.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::srand(seed);

    // Generate waveform.
    constexpr int32_t sample_rate = 44100;
    constexpr int duration_seconds = 10;
    const int sample_count = sample_rate * duration_seconds;
    std::vector<float> samples(sample_count);

    // Oscillator time_modulator(OscillatorType::Sine, 0.05f, 44100.0f);
    // Oscillator phase_modulator(OscillatorType::Sine, 5.0f, 44100.0f);
    // Oscillator voice(OscillatorType::Square, 220.0f, 44100.0f);

    // for (int i = 0; i < sample_count; i++)
    // {
    //     voice.Speed = 1.0f + time_modulator.Sample();
    //     phase_modulator.Speed = 1.0f + time_modulator.Sample();
    //     voice.PhaseOffset = phase_modulator.Sample();
    //     samples[i] = voice.Sample();
    // }

    SinglePole low_pass(FilterType::LowPass, 0.05);
    for (int i = 0; i < sample_count; i++)
    {
        samples[i] = low_pass.Filter(Noise());
    }

    // Export to WAV.
    export_wav("noise_filtered.wav", 1, sample_rate, samples);
    return 0;
}
