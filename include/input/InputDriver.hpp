#pragma once

#include <string>

enum InputDriverMode
{
    Poll,
    Callback
};

class InputDriver
{
    public:
        InputDriverMode Mode;   // Data access mode.
        std::string Id;         // Driver identifier used for unique device ids.

        // Invoked on driver registration.
        virtual void Initialize() = 0;
    
        virtual ~InputDriver() = default;
};