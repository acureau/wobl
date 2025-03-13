#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include "Wav.hpp"

void export_wav(const std::string& file_path, int16_t channel_count, int32_t sample_rate, const std::vector<float>& data)
{
    // Open file for writing.
    std::ofstream wav_file(file_path, std::ios::binary);
    if (!wav_file)
    {
        std::cerr << "Error: Could not write WAV file, unable to open file path: " << file_path << "\n";
        return;
    }

    // Calculate required parameters.
    constexpr int16_t bits_per_sample = 32;
    constexpr int16_t format_tag = 3;
    const int32_t data_size = static_cast<int32_t>(data.size() * sizeof(float));
    const int32_t chunk_size = data_size + 44 - 8;
    constexpr int32_t fmt_chunk_size = 16;
    const int16_t bytes_per_block = channel_count * (bits_per_sample / 8);
    const int32_t byte_rate = sample_rate * bytes_per_block;


    // Write RIFF header.
    wav_file.write("RIFF", 4);
    wav_file.write(reinterpret_cast<const char*>(&chunk_size), 4);
    wav_file.write("WAVE", 4);

    // Write format chunk.
    wav_file.write("fmt ", 4);
    wav_file.write(reinterpret_cast<const char*>(&fmt_chunk_size), 4);
    wav_file.write(reinterpret_cast<const char*>(&format_tag), 2);
    wav_file.write(reinterpret_cast<const char*>(&channel_count), 2);
    wav_file.write(reinterpret_cast<const char*>(&sample_rate), 4);
    wav_file.write(reinterpret_cast<const char*>(&byte_rate), 4);
    wav_file.write(reinterpret_cast<const char*>(&bytes_per_block), 2);
    wav_file.write(reinterpret_cast<const char*>(&bits_per_sample), 2);

    // Write data chunk.
    wav_file.write("data", 4);
    wav_file.write(reinterpret_cast<const char*>(&data_size), 4);
    wav_file.write(reinterpret_cast<const char*>(data.data()), data_size);

    // Close file.
    wav_file.close();
}