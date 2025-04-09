#include "WindowsWasapi.hpp"
#include "OutputDevice.hpp"
#include "OutputFormat.hpp"

int main()
{
    // Initialize driver.
    WindowsWasapi output_driver;
    output_driver.Initialize();

    // Enumerate devices.
    std::vector<OutputDevice> devices = output_driver.EnumerateDevices();
    
    // Enable devices.
    OutputFormat requested_output_format
    {
        44100,
        32,
        SampleType::Integer,
        SampleSign::Signed,
        3
    };
    output_driver.EnableDevice(devices[0], requested_output_format);
    
    // Disable devices.
    output_driver.DisableDevice(devices[0]);
    return 0;
}