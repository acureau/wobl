#pragma once

#include <string>
#include <optional>
#include <unordered_map>
#include "Input.hpp"

class InputDevice
{
    std::string Id;
    std::optional<std::string> Name;
    std::unordered_map<std::string, Input> Inputs;
};