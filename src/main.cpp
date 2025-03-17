#include <vector>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include "Oscillator.hpp"
#include "Filter.hpp"
#include "Noise.hpp"
#include "Wav.hpp"
#include "Envelope.hpp"

#include "WASAPI.hpp"    

std::vector<float> Callback(DeviceParams params, int frame_count)
{
    return std::vector<float>();
}

int main()
{
    // Initialize RNG.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::srand(seed);

    // Initialize sound API.
    WASAPI sound_api;
    std::vector<DeviceIdentity> audio_devices = sound_api.GetDevices();
    sound_api.SetDevice(audio_devices[1]);
    sound_api.Initialize();
    sound_api.Start(Callback);
    // // Generate waveform.
    // int duration_seconds = 10;
    // int sample_rate = 44100;
    // int channel_count = 2;
    // int sample_count = sample_rate * duration_seconds; // Samples per channel
    // std::vector<float> samples(sample_count * channel_count); // Interleaved buffer

    // // Create oscillators & envelope
    // Oscillator modulator(OscillatorType::Sine, 20.0f, static_cast<float>(sample_rate));
    // Oscillator voice(OscillatorType::Square, 440.0f, static_cast<float>(sample_rate));
    // ADSR envelope(static_cast<float>(sample_rate), 1.0f, 1.0f, 0.5f, 1.0f);
    // envelope.Trigger();

    // // Generate interleaved samples
    // for (int i = 0; i < sample_count; i++)
    // {
    //     if (i == sample_rate * 5)
    //     {
    //         envelope.Release();
    //     }

    //     if (i == sample_rate * 7)
    //     {
    //         envelope.Trigger();
    //     }

    //     // Generate the same audio for each channel
    //     voice.PhaseOffset = modulator.Sample();
    //     float sample_value = envelope.GetLevel() * voice.Sample();

    //     // Interleave samples into buffer
    //     for (int ch = 0; ch < channel_count; ch++)
    //     {
    //         samples[i * channel_count + ch] = sample_value; // Same sample for all channels
    //     }
    // }

    // // Export to WAV.
    // export_wav("square_envelope.wav", channel_count, sample_rate, samples);
    return 0;
}
