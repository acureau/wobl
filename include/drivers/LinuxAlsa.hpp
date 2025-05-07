#ifdef PLATFORM_LINUX

#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <memory>
#include <functional>
#include <unordered_map>
#include <alsa/asoundlib.h>
#include "OutputFormat.hpp"
#include "OutputDriver.hpp"

using SampleCallback = std::function<std::vector<std::byte>(const std::string&, const std::string&)>;

class LinuxAlsa : public OutputDriver
{
    private:
        // A list used to convert internal format objects to ALSA format enums.
        static const std::vector<std::pair<std::string, snd_pcm_format_t>> SampleFormatEnumPairs;
        static std::string OutputFormatToEnumKey(const OutputFormat &output_format);
        static OutputFormat FormatEnumKeyToOutputFormat(const std::string &format_enum_key);
        static OutputFormat FormatEnumToOutputFormat(snd_pcm_format_t format_enum);
        static std::vector<snd_pcm_format_t> GetOrderedFormatNegotiationEnums(const OutputFormat &output_format);
        
        // Map of device IDs to the corresponding handles.
        std::unordered_map<std::string, snd_pcm_t*> DeviceIDHandleMap;

    public:
        void Initialize(std::shared_ptr<SampleCallback> sample_callback_pointer) override;
        std::vector<OutputDevice> EnumerateDevices() override;
        OutputFormat EnableDevice(const OutputDevice &device, const OutputFormat &requested_output_format) override;
        void DisableDevice(const OutputDevice &device) override;
        void Flush() override;
        ~LinuxAlsa() override;
        LinuxAlsa();
};

#endif