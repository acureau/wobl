#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include "OutputFormat.hpp"
#include "OutputDevice.hpp"
#include "OutputDriver.hpp"

class WindowsWasapi : public OutputDriver
{
    public:
        std::string Id = "Windows WASAPI Driver"; // todo: we're hiding the base, won't be accessible through the interface. fix this in all drivers.
        void Initialize() override;
        std::vector<OutputDevice> EnumerateDevices() override;
        OutputFormat EnableDevice(const OutputDevice &device, const OutputFormat &requested_output_format) override;
        void DisableDevice(const OutputDevice &device) override;
        void ReceiveSampleFrame(const OutputDevice &device, const std::vector<std::byte> &frame) override;
        void Flush() override;
        ~WindowsWasapi() override;
};
