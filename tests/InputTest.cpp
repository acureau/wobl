#include <iostream>
#include "InputHandler.hpp"

int main()
{
    // Configure input handler callback.
    InputHandler input_handler;
    input_handler.RegisterInputEventCallback
    (
        std::make_shared<InputEventCallback>
        (
            [](std::queue<InputEvent> input_events)
            {
                while (!input_events.empty())
                {
                    InputEvent input_event = input_events.front();
                    std::cout << "[InputEvent] Device: " << input_event.DeviceId;
                    if (input_event.DeviceName)
                    {
                        std::cout << " (" << *input_event.DeviceName << ")";
                    }
                    std::cout << ", Name: " << input_event.Name;
                    std::cout << ", Type: " << (input_event.Type == Binary ? "Binary" : "Analog");
                    std::cout  << ", State: " << input_event.State << std::endl;
                    input_events.pop();
                }
            }
        )
    );

    // Waste cycles on main thread.
    while (true) { }
}