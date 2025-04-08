#include <iostream>
#include "OutputHandler.hpp"

bool OutputHandler::ValidateFormat(const OutputFormat &format)
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

bool OutputHandler::ValidateSampleFrame(const std::vector<std::byte> &frame)
{
    int bytes_per_frame = (this->MasterFormat.ChannelCount * this->MasterFormat.BitsPerSample) / 8;
    if (frame.size() != bytes_per_frame)
    {
        std::cout << "Sample frame size does not match master format." << std::endl;
        return false;
    }
    return true;
}

void OutputHandler::SetMasterFormat(OutputFormat master_format)
{
    // Validate passed format.
    if (!this->ValidateFormat(master_format))
    {
        std::cout << "Unable to set master format." << std::endl;
        return;
    }

    // Set master format.
    this->MasterFormat = master_format;

    // Determine frame count for 10ms sample buffer.
    int frame_count = master_format.SampleRate / 100;
    
    // Determine bytes per frame.
    int bytes_per_frame = (master_format.BitsPerSample * master_format.ChannelCount) / 8;
    
    // Resize sample buffer.
    this->SampleBuffer.resize(frame_count * bytes_per_frame);

    // Reset frame index.
    this->MasterFrameIndex = 0;
}

OutputHandler::OutputHandler(OutputFormat master_format)
{
    this->SetMasterFormat(master_format);

    // todo: proceed to register drivers
}

/*
    UNSORTED SCRATCH CODE
*/

uint32_t BytesToWord(std::vector<std::byte> bytes)
{
    uint32_t word = 0;
    for (int byte_index = 0; byte_index < bytes.size(); byte_index++)
    {
        word |= static_cast<uint32_t>(bytes[byte_index]) << (byte_index * 8);
    }
    return word;
}

float NormalizeSample(uint32_t sample, OutputFormat format)
{
    return 0.0f;
}

uint32_t MapNormal(float normal, OutputFormat format)
{
    return 0;
}

std::vector<std::byte> WordToBytes(uint32_t word)
{
    return std::vector<std::byte>();
}

std::vector<std::byte> ConvertSamples(const std::vector<std::byte> &samples, const OutputFormat &source_format, const OutputFormat &destination_format)
{
    std::vector<std::byte> converted_samples;

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

            // Convert sample bytes to word.
            uint32_t raw_sample = BytesToWord(sample_bytes);

            // Normalize sample.
            float normalized_sample = NormalizeSample(raw_sample, source_format);

            // Map normalized sample.
            uint32_t mapped_sample = MapNormal(normalized_sample, destination_format);

            // One to determine the "range" of a data type from the output format. Or really, the "position in the range" as a percentage.
            // One to produce a uint32_t given an output format and this "position in the range".
        }
    }

    return converted_samples;
}