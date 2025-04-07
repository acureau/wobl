#include <iostream>
#include <cstdint>
#include "OutputConverter.hpp"

bool OutputConverter::ValidateFormat(const OutputFormat &format)
{
    if (format.BitsPerSample % 8 > 0)
    {
        std::cout << "Unsupported bits per sample '" << format.BitsPerSample << "'. Is not a multiple of 8." << std::endl;
        return false;
    }

    if (format.BitsPerSample > 32 || format.BitsPerSample < 8)
    {
        std::cout << "Unsupported bits per sample '" << format.BitsPerSample << "'. Is not between 8 and 32." << std::endl;
        return false;
    }

    if (format.Type == SampleType::FloatingPoint && format.Sign == SampleSign::Unsigned)
    {
        std::cout << "Floating point samples cannot be unsigned." << std::endl;
        return false;
    }

    if (format.SampleRate <= 0)
    {
        std::cout << "Sample rate must be greater than zero." << std::endl;
        return false;
    }

    if (format.ChannelCount <= 0)
    {
        std::cout << "Channel count must be greater than zero." << std::endl;
        return false;
    }

    return true;
}

bool OutputConverter::ValidateSourceSize(const std::vector<std::byte> &source_samples, const OutputFormat &source_format)
{
    size_t bits_per_frame = source_format.ChannelCount * source_format.BitsPerSample;
    size_t total_bits = source_samples.size() * 8;
    if (!(total_bits >= bits_per_frame && total_bits % bits_per_frame == 0))
    {
        std::cout << "Source samples do not contain one or a whole number of sample frames." << std::endl;
        return false;
    }
    return true;
}

std::vector<std::byte> OutputConverter::ConvertSamples(const std::vector<std::byte> &samples, const OutputFormat &source_format, const OutputFormat &destination_format)
{
    std::vector<std::byte> converted_samples;

    // Validate samples vector size, output formats.
    if (ValidateSourceSize(samples, source_format) && ValidateFormat(source_format) && ValidateFormat(destination_format))
    {
        // Calculate bytes per sample and channel.
        int bytes_per_sample = source_format.BitsPerSample / 8;
        int bytes_per_frame = bytes_per_sample * source_format.ChannelCount;

        // Iterate over all sample frames.
        for (int frame_index = 0; frame_index < samples.size(); frame_index += bytes_per_frame)
        {
            // Iterate over samples in sample frame.
            for (int sample_index = 0; sample_index < bytes_per_frame; sample_index += bytes_per_sample)
            {
                // Get the sample from the samples vector.
                int sample_offset = frame_index + sample_index;
                std::vector<std::byte> sample_bytes(samples.begin() + sample_offset, samples.begin() + sample_offset + bytes_per_sample);

                // Convert sample to uint32_t. Remember, this represents a 32-bit word, not an unsigned 32-bit integer.
                uint32_t raw_sample = 0;
                for (int byte_index = 0; byte_index < sample_bytes.size(); byte_index++)
                {
                    raw_sample |= static_cast<uint32_t>(sample_bytes[byte_index]) << (byte_index * 8);
                }

                // We need to prepare the data for conversion now. There are three cases:
                //  - The data is an unsigned integer, it's ready for conversion.
                //  - The data is a signed integer, we need to sign extend.
                //  - The data is a float, we need to cast.
                if (source_format.Type == SampleType::FloatingPoint)
                {

                }

                else if (source_format.Sign == SampleSign::Signed)
                {

                }

                else
                {

                }
            }
        }
    }

    return converted_samples;
}