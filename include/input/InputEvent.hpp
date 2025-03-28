#pragma once

#include <string>
#include <optional>

enum InputType
{
    Binary,
    Analog
};

struct InputEvent
{
    std::string DeviceId;
    std::optional<std::string> DeviceName;
    std::string Name;
    InputType Type;
    float State;
};