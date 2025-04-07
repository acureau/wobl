#pragma once

#include <cstddef>
#include <vector>
#include "OutputFormat.hpp"

class OutputConverter
{
    private:
        // Validates that output formats are supported by the converter.
        bool ValidateFormat(const OutputFormat &format);

        // Validates that the samples contain a whole number of sample frames.
        bool ValidateSourceSize(const std::vector<std::byte> &source_samples, const OutputFormat &source_format);

    public:
        // Converts samples from one output format to another. 
        std::vector<std::byte> ConvertSamples(const std::vector<std::byte> &samples, const OutputFormat &source_format, const OutputFormat &destination_format);
};