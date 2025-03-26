#ifdef PLATFORM_WINDOWS

#include <iostream>
#include <vector>
#include <string>

#include "Windows.h"

#include "WindowsRawInput.hpp"

std::string GetDeviceName(DWORD device_type)
{
    static int MouseCount = 0;
    static int KeyboardCount = 0;
    static int HIDCount = 0;

    switch (device_type)
    {
        case (RIM_TYPEMOUSE):
            MouseCount++;
            return "Mouse #" + std::to_string(MouseCount);
        case (RIM_TYPEKEYBOARD):
            KeyboardCount++;
            return "Keyboard #" + std::to_string(KeyboardCount);
        default:
            HIDCount++;
            return "Generic Input Device #" + std::to_string(HIDCount);
    }
}

std::string GetDevicePath(HANDLE raw_input_device)
{
    // Get the size of the device path.
    UINT name_size;
    if (GetRawInputDeviceInfo(raw_input_device, RIDI_DEVICENAME, nullptr, &name_size) < 0)
    {
        std::cout << "Unable to get the size of the device path." << std::endl;
        return "";
    }
    
    // Get the device path.
    std::vector<char> name_buffer(name_size);
    if (GetRawInputDeviceInfo(raw_input_device, RIDI_DEVICENAME, name_buffer.data(), &name_size) < 0)
    {
        std::cout << "Unable to get the device path." << std::endl;
        return "";
    }
    
    return std::string(name_buffer.data());
}

void WindowsRawInput::Initialize()
{
    // Get a handle to the main program executable.
    HINSTANCE GetModuleHandle(NULL);

    // Create and register window class attributes.
    WNDCLASS window_class_attributes {};
    // window_class_attributes.

    // Determine the number of raw input devices.
    UINT device_count;
    if (GetRawInputDeviceList(NULL, &device_count, sizeof(RAWINPUTDEVICELIST)) != 0)
    {
        std::cout << "Unable to get the number of raw input devices." << std::endl;
    }
    else
    {
        std::cout << "There are " << device_count << " raw input devices." << std::endl;
    }

    // Get the raw input devices.
    RAWINPUTDEVICELIST raw_input_device_list[device_count];
    if (GetRawInputDeviceList(raw_input_device_list, &device_count, sizeof(RAWINPUTDEVICELIST)) != device_count)
    {
        std::cout << "Unable to retrieve all raw input devices." << std::endl;
    }

    // Iterate over all input devices.
    for (int i = 0; i < device_count; i++)
    {
        // Print the device path.
        std::cout << "NAME: " << GetDeviceName(raw_input_device_list[i].dwType) << std::endl;
        std::cout << "PATH: " << GetDevicePath(raw_input_device_list[i].hDevice) << std::endl << std::endl;
    }

}

#endif