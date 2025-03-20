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
    Oscillator modulator(OscillatorType::Sine, 5.0f, static_cast<float>(sample_rate));
    Oscillator voice(OscillatorType::Sine, 220.0f, static_cast<float>(sample_rate));
    ADSR voice_envelope(static_cast<float>(sample_rate), 0.5f, 1.0f, 1.0f, 1.0f);
    ADSR mod_envelope(static_cast<float>(sample_rate), 3.0f, 1.0f, 1.0f, 1.0f);
    ADSR noise_envelope(static_cast<float>(sample_rate), 0.05f, 1.5f, 0.0f, 0.0f);
    SinglePole noise_filter(FilterType::LowPass, 0.05);
    
    // Generate interleaved samples
    bool modulator_enabled = false;
    for (int i = 0; i < sample_count; i++)
    {

        if (i == 0)
        {
            voice_envelope.Trigger();
            noise_envelope.Trigger();
        }

        if (i == sample_rate * 0.33)
        {
            voice.SetFrequency(246.94f);
            voice_envelope.Trigger();
            noise_envelope.Trigger();
        }

        if (i == sample_rate * 0.66)
        {
            voice.SetFrequency(261.62f);
            voice_envelope.Trigger();
            noise_envelope.Trigger();
        }

        if (i == sample_rate * 1.0)
        {
            voice.SetFrequency(293.66f);
            voice_envelope.Trigger();
            noise_envelope.Trigger();

            mod_envelope.Trigger();
            modulator_enabled = true;
        }

        if (i == sample_rate * 2)
        {
            voice_envelope.Release();
        }

        // Generate the same audio for each channel
        if (modulator_enabled)
        {
            voice.PhaseOffset = mod_envelope.GetLevel() * modulator.Sample();
        }

        float noise_value = Noise();
        float sample_value = (0.985 * (voice_envelope.GetLevel() * voice.Sample())) + (0.015 * (noise_filter.Filter(noise_value) * noise_envelope.GetLevel()));

        // Interleave samples into buffer
        for (int ch = 0; ch < channel_count; ch++)
        {
            samples[i * channel_count + ch] = sample_value; // Same sample for all channels
        }
    }

    // Export to WAV.
    export_wav("sine_scale_noise.wav", channel_count, sample_rate, samples);
    return 0;
}
