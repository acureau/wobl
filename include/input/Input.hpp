#pragma once

#include <string>
#include <optional>
#include <variant>

using InputValue = std::variant<bool, float, std::pair<float, float>>;

enum InputType
{
    Binary,
    Analog,
    Coordinate
};

class Input
{
    std::string Id;
    std::optional<std::string> Name;
    InputType Type;
    InputValue State;
};