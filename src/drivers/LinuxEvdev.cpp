#ifdef PLATFORM_LINUX

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <utility>

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include "LinuxEvdev.hpp"

using StringPairVector = std::vector<std::pair<std::string, std::string>>;

StringPairVector EnumerateDevices()
{
    StringPairVector devices;

    // Get all input events in '/dev/input'.
    for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator("/dev/input"))
    {
        // Get file path, filter non-event paths.
        std::string file_path = entry.path().string();
        if (file_path.contains("event"))
        {
            // Try to open event path.
            int event_path_descriptor = open(file_path.c_str(), O_RDONLY);
            if (errno == EACCES)
            {
                std::cout << "User lacks permissions to open '/dev/input/eventX' files. Add user to 'input' group and restart session." << std::endl;
                break;
            }
            
            // Get human-readable device name.
            char device_name_buffer[256];
            if (ioctl(event_path_descriptor, EVIOCGNAME(sizeof(device_name_buffer)), device_name_buffer) < 0)
            {
                std::cout << "Unable to get device name for '" << file_path << "'." << std::endl;
                continue;
            }
            else
            {
                // Construct and insert device path : name pair.
                std::string device_name(device_name_buffer);
                devices.emplace_back(file_path, device_name);
            }

            // Close event path.
            close(event_path_descriptor);
        }
    }

    return devices;
}

void LinuxEvdev::Initialize()
{
    StringPairVector devices = EnumerateDevices();
    return;
}

#endif