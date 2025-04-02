/*
    Driver Type: Input
    Driver Target: Windows Raw Input API
    
    Notes:
    The Raw Input API is confusing in that "registering raw input devices" does not refer to registering individual devices.
    A RAWINPUTDEVICE struct contains a set of filters used to select matching devices to forward events from.
*/

#ifdef PLATFORM_WINDOWS
#pragma once

#include <queue>
#include <Windows.h>
#include "InputDriver.hpp"
#include "InputEvent.hpp"

class WindowsRawInput : public InputDriver
{
    private:
        

    public:
        void Initialize();
        std::queue<InputEvent> GetInputEvents();
};

#endif