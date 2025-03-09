#include <stdio.h>
#include <stdint.h>
#include "wav.h"

void export_wav(const char* file_path, int16_t channel_count, int32_t sample_rate, int32_t data_size, const float* data)
{
    // Open file for writing.
    FILE* wav_file = fopen(file_path, "wb");
    if (!wav_file) {
        perror("Could not write WAV file, unable to open file path.");
        return;
    }
    
    // Calculate required parameters.
    const int16_t bits_per_sample = 32;
    int32_t chunk_size = data_size + 44 - 8;
    int32_t fmt_chunk_size = 16;
    int16_t format_tag = 3;
    int16_t bytes_per_block = channel_count * bits_per_sample / 8;
    int32_t byte_rate = sample_rate * bytes_per_block;

    // Write RIFF header.
    fwrite("RIFF", 4, 1, wav_file);
    fwrite(&chunk_size, 4, 1, wav_file);
    fwrite("WAVE", 4, 1, wav_file);

    // Write format chunk.
    fwrite("fmt ", 4, 1, wav_file);
    fwrite(&fmt_chunk_size, 4, 1, wav_file);
    fwrite(&format_tag, 2, 1, wav_file);
    fwrite(&channel_count, 2, 1, wav_file);
    fwrite(&sample_rate, 4, 1, wav_file);
    fwrite(&byte_rate, 4, 1, wav_file);
    fwrite(&bytes_per_block, 2, 1, wav_file);
    fwrite(&bits_per_sample, 2, 1, wav_file);

    // Write data chunk.
    fwrite("data", 4, 1, wav_file);
    fwrite(&data_size, 4, 1, wav_file);
    fwrite(data, data_size, 1, wav_file);

    // Close file.
    fclose(wav_file);
}