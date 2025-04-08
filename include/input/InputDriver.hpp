#pragma once

#include <string>
#include <queue>
#include "InputEvent.hpp"

class InputDriver
{
    public:
        // Driver identifier.
        std::string Id;

        // Invoked on driver registration.
        virtual void Initialize() = 0;

        // Poll for input events.
        virtual std::queue<InputEvent> GetInputEvents() = 0;
    
        // Purely virtual class, call implementation destructor.
        virtual ~InputDriver() = default;
};