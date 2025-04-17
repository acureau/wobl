#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <memory>
#include <functional>
#include "OutputFormat.hpp"
#include "OutputDevice.hpp"

using SampleCallback = std::function<std::vector<std::byte>(const std::string&, const std::string&)>;

class OutputDriver
{
    public:
        // Driver identifier.
        std::string Id;
    
        // Sample callback pointer.
        std::shared_ptr<SampleCallback> SampleCallbackPointer;

        // Invoked on driver registration.
        virtual void Initialize(std::shared_ptr<SampleCallback> sample_callback_pointer) = 0;

        // Enumerate all devices supported by driver.
        virtual std::vector<OutputDevice> EnumerateDevices() = 0;

        // Enable device playback, negotiating output format.
        virtual OutputFormat EnableDevice(const OutputDevice &device, const OutputFormat &requested_output_format) = 0;
    
        // Disable device playback.
        virtual void DisableDevice(const OutputDevice &device) = 0;

        // Method invoked by the output handler thread loop, requests playback of recieved samples.
        virtual void Flush() = 0;

        // Purely virtual class, call implementation destructor.
        virtual ~OutputDriver() = default;
};