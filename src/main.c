#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "oscillators.h"
#include "wav.h"

int main()
{
    // Initialize RNG.
    srand(time(NULL));

    // Generate waveform.
    const int32_t sample_rate = 44100;
    int sample_count = sample_rate * 20;
    float samples[sample_count];
    for (int i = 0; i < sample_count; i++)
    {
        float time = (float)i / sample_rate;
        float phase_modulator = sine(10.0f, time, 0.0f);
        float mix_modulator = (sine(0.2f, time, 0.0f) + 1.0f) * 0.5f;
        samples[i] = (sine(440.0f, time, phase_modulator) * mix_modulator) + (sawtooth(340.0f, time, phase_modulator) * (1.0f - mix_modulator));
    }

    // Export to WAV.
    export_wav("sine.wav", 1, sample_rate, sample_count * sizeof(float), samples);

    return 0;
}