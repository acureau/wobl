#pragma once

#include <string>
#include <optional>

class OutputDevice 
{
    std::string DriverId;
    std::string DeviceId;
    std::optional<std::string> DeviceName;
};