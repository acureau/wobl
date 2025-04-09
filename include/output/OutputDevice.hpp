#pragma once

#include <string>
#include <optional>

struct OutputDevice 
{
    std::string DriverId;
    std::string DeviceId;
    std::optional<std::string> DeviceName;
};