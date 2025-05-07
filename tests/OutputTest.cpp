#include <memory>
#include <vector>
#include <chrono>
#include <cstring>
#include "OutputDevice.hpp"
#include "OutputFormat.hpp"
#include "OutputHandler.hpp"
#include "Oscillator.hpp"

#ifdef PLATFORM_WINDOWS
#include "WindowsWasapi.hpp"
#endif
#ifdef PLATFORM_LINUX
#include "LinuxAlsa.hpp"
#endif 

int main()
{
    // Set shared params.
    int SampleRate = 44100;
    int ChannelCount = 2;

    // Initialize oscillator.
    Oscillator sine(OscillatorType::Sine, 440.0f, SampleRate);

    // Initialize platform-specific driver.
    std::unique_ptr<OutputDriver> output_driver;
    #ifdef PLATFORM_WINDOWS
    output_driver = std::make_unique<WindowsWasapi>();
    #endif
    #ifdef PLATFORM_LINUX
    output_driver = std::make_unique<LinuxAlsa>();
    #endif 

    // Initialize driver.
    output_driver->Initialize
    (
        std::make_shared<SampleCallback>
        (
            [sine](const std::string& driver_id, const std::string& device_id) mutable -> std::vector<std::byte>
            {
                float sample = sine.Sample();
                std::vector<std::byte> buffer(sizeof(float) * 2);
                std::memcpy(buffer.data(), &sample, sizeof(float));
                std::memcpy(buffer.data() + sizeof(float), &sample, sizeof(float));
                return buffer;
            }
        )
    );

    // Enumerate devices.
    std::vector<OutputDevice> devices = output_driver->EnumerateDevices();
    
    // Enable devices.
    OutputFormat requested_output_format
    {
        SampleRate,
        32,
        SampleType::FloatingPoint,
        SampleSign::Signed,
        ChannelCount
    };

    for (OutputDevice &device : devices)
    {
        #ifdef PLATFORM_WINDOWS
        output_driver->EnableDevice(device, requested_output_format);
        break;
        #endif
        #ifdef PLATFORM_LINUX
        if (device.DeviceId == "default")
        {
            output_driver->EnableDevice(device, requested_output_format);
        }
        #endif
    }
    
    // 10s flush loop.
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(10))
    {
        output_driver->Flush();
    }

    // Disable devices.
    output_driver->DisableDevice(devices[0]);
    return 0;
}