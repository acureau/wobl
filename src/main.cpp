#include <vector>
#include <cstdlib>
#include <chrono>
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
    constexpr int32_t sample_rate = 44100;
    constexpr int duration_seconds = 10;
    const int sample_count = sample_rate * duration_seconds;
    std::vector<float> samples(sample_count);

    Oscillator phase_modulator(OscillatorType::Sine, 20.0f, 44100.0f);
    Oscillator voice(OscillatorType::Square, 440.0f, 44100.0f);
    ADSR envelope(44100.0f, 1.0f, 1.0f, 0.5f, 1.0f);
    envelope.Trigger();

    for (int i = 0; i < sample_count; i++)
    {
        if (i == sample_rate * 5)
        {
            envelope.Release();
        }

        voice.PhaseOffset = phase_modulator.Sample();
        samples[i] = envelope.GetLevel() * voice.Sample();
    }

    // Export to WAV.
    export_wav("square_envelope.wav", 1, sample_rate, samples);
    return 0;
}
