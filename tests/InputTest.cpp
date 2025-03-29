#include <iostream>

#ifdef PLATFORM_WINDOWS

#include "WindowsRawInput.hpp"

int main()
{
    WindowsRawInput driver;
    driver.Initialize();
    return 0;
}

#endif

#ifdef PLATFORM_LINUX

#include "LinuxEvdev.hpp"

int main()
{
    // Initialize evdev driver.
    LinuxEvdev driver;
    driver.Initialize();

    // Simulate poll loop.
    while (true)
    {
        std::queue<InputEvent> input_events = driver.GetInputEvents();
        while (!input_events.empty())
        {
            InputEvent input_event = input_events.front();
            std::cout << "[InputEvent] Device: " << input_event.DeviceId;
            if (input_event.DeviceName)
                std::cout << " (" << *input_event.DeviceName << ")";
            std::cout << ", Name: " << input_event.Name
                    << ", Type: " << (input_event.Type == Binary ? "Binary" : "Analog")
                    << ", State: " << input_event.State << std::endl;
            input_events.pop();
        }
    }
}

#endif