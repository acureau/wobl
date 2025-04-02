#ifdef PLATFORM_WINDOWS

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>

#include "Windows.h"
#include "hidusage.h"

#include "InputEvent.hpp"
#include "WindowsRawInput.hpp"

/*
    We're going to standardize the event format on floats I guess, since they can contain whole numbers as well.
    We're going to isolate each input on a device, we can generate multiple input events per device input event.
*/

std::queue<InputEvent> InputEventQueue;

void HandleRawKeyboardEvent(const RAWKEYBOARD &raw_keyboard_event)
{
    // Ignore invalid states.
    if (raw_keyboard_event.MakeCode == KEYBOARD_OVERRUN_MAKE_CODE || raw_keyboard_event.VKey >= UCHAR_MAX)
    {
        return;
    }

    // 

    std::cout << "keyboard event" << std::endl;
}

LRESULT CALLBACK WindowProceedure(HWND window_handle, UINT message, WPARAM w_parameter, LPARAM l_parameter) {
    // Handle WM_INPUT events, fired by the Raw Input API.
    if (message == WM_INPUT)
    {
        // Cast param to raw input handle.
        HRAWINPUT raw_input_handle = reinterpret_cast<HRAWINPUT>(l_parameter);

        // Get the size of the raw input structure.
        UINT raw_input_size = 0;
        GetRawInputData(raw_input_handle, RID_INPUT, NULL, &raw_input_size, sizeof(RAWINPUTHEADER));

        // Get the raw input structure.
        // HID raw data is variable-length, need to allocate memory.
        std::vector<std::byte> raw_input_buffer(raw_input_size);
        GetRawInputData(raw_input_handle, RID_INPUT, raw_input_buffer.data(), &raw_input_size, sizeof(RAWINPUTHEADER));
        RAWINPUT* raw_input = reinterpret_cast<RAWINPUT*>(raw_input_buffer.data());

        // Identify event type and pass to proper handler.
        switch (raw_input->header.dwType)
        {
            case RIM_TYPEKEYBOARD:
                HandleRawKeyboardEvent(raw_input->data.keyboard);
                break;
        }
    }

    // Pass events to the default window proceedure.
    return DefWindowProc(window_handle, message, w_parameter, l_parameter);
}

HWND InitializeHiddenWindow()
{
    // Name the window class.
    const char* class_name = "HiddenWindowClass";

    // Get handle to the main executable module.
    HINSTANCE main_executable_module = GetModuleHandle(NULL);
    if (main_executable_module == NULL)
    {
        std::cout << "Unable to get the main executable module handle." << std::endl;
        return NULL;
    }

    // Create and window class attributes.
    WNDCLASS window_class_attributes {};
    window_class_attributes.lpfnWndProc = WindowProceedure;
    window_class_attributes.hInstance = main_executable_module;
    window_class_attributes.lpszClassName = class_name;

    // Register class.
    if (RegisterClass(&window_class_attributes) == 0)
    {
        std::cout << "Unable to register hidden window class." << std::endl;
        return NULL;
    }

    // Create window.
    HWND HiddenWindow = CreateWindowEx(0, class_name, "", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, main_executable_module, NULL);
    if (HiddenWindow == NULL)
    {
        std::cout << "Unable to create hidden window." << std::endl;
        return NULL;
    }

    // Return success.
    return HiddenWindow;
}

bool ProcessMessageQueue()
{
    MSG message;
    BOOL status = GetMessage(&message, NULL, 0, 0);
    if (status < 0)
    {
        std::cout << "Message queue reading error." << std::endl;
        return false;
    }
    else if (status == 0)
    {
        std::cout << "Message queue exit event received." << std::endl;
        return false;
    }
    else
    {
        DispatchMessage(&message);
        return true;
    }
}

struct WindowsRawInputDevice
{
    std::string ID;
    std::string Name;
    HANDLE Handle;
};

std::string GetDeviceID(HANDLE device_handle)
{
    // Get the size of the device id.
    UINT name_size;
    if (GetRawInputDeviceInfo(device_handle, RIDI_DEVICENAME, nullptr, &name_size) < 0)
    {
        std::cout << "Unable to get the size of the device id." << std::endl;
        return "";
    }
    
    // Get the device id.
    std::vector<char> name_buffer(name_size);
    if (GetRawInputDeviceInfo(device_handle, RIDI_DEVICENAME, name_buffer.data(), &name_size) < 0)
    {
        std::cout << "Unable to get the device id." << std::endl;
        return "";
    }
    
    return std::string(name_buffer.data());
}

RID_DEVICE_INFO GetDeviceInfo(HANDLE device_handle)
{
    UINT device_info_size = sizeof(RID_DEVICE_INFO);
    RID_DEVICE_INFO device_info {};
    device_info.cbSize = device_info_size;
    if (GetRawInputDeviceInfo(device_handle, RIDI_DEVICEINFO, &device_info, &device_info_size) < 0)
    {
        std::cout << "Unable to get the device info." << std::endl;
        device_info.dwType = -1;
    }
    return device_info;
}

std::string GetDeviceName(DWORD device_type, bool reset = false)
{
    static int MouseCount = 0;
    static int KeyboardCount = 0;
    static int HIDCount = 0;

    if (reset)
    {
        MouseCount = 0;
        KeyboardCount = 0;
        HIDCount = 0;
        return "";
    }

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

bool IsDeviceSupported(RAWINPUTDEVICELIST enumerated_device)
{
    // Currently keyboards are supported.
    return enumerated_device.dwType == RIM_TYPEKEYBOARD;
}

std::vector<WindowsRawInputDevice> EnumerateDevices()
{
    std::vector<WindowsRawInputDevice> devices;

    // Reset enumeration state.
    GetDeviceName(0, true);

    // Determine the number of raw input devices.
    UINT device_count;
    if (GetRawInputDeviceList(NULL, &device_count, sizeof(RAWINPUTDEVICELIST)) != 0)
    {
        std::cout << "Unable to get the number of raw input devices." << std::endl;
    }
    else if (device_count == 0)
    {
        std::cout << "Found zero raw input devices." << std::endl;
    }
    else
    {
        // Get the raw input devices.
        RAWINPUTDEVICELIST raw_input_device_list[device_count];
        if (GetRawInputDeviceList(raw_input_device_list, &device_count, sizeof(RAWINPUTDEVICELIST)) != device_count)
        {
            std::cout << "Unable to retrieve all raw input devices." << std::endl;
        }
        else
        {
            // Iterate over all input devices.
            for (int i = 0; i < device_count; i++)
            {
                // Determine if device is supported by the driver.
                if (IsDeviceSupported(raw_input_device_list[i]))
                {
                    // Retrieve device id.
                    std::string device_id = GetDeviceID(raw_input_device_list[i].hDevice);
                    if (device_id == "")
                    {
                        std::cout << "Unable to retrieve device id." << std::endl;
                        continue;
                    }
                    
                    // Retrieve device name.
                    std::string device_name = GetDeviceName(raw_input_device_list[i].dwType);
                    
                    // Create and add device structure to list.
                    devices.emplace_back(device_id, device_name, raw_input_device_list[i].hDevice);
                }
            }
        }
    }

    return devices;
}

bool RegisterDevices(HWND window_handle, const std::vector<WindowsRawInputDevice>& devices)
{
    // Store unique sets of parameters, created raw input device structs.
    std::set<std::tuple<USHORT, USHORT, DWORD>> unique_parameters;
    std::vector<RAWINPUTDEVICE> raw_input_devices;

    // Iterate over devices list.
    for (WindowsRawInputDevice device : devices)
    {
        // Get and validate device info is of valid type.
        RID_DEVICE_INFO device_info = GetDeviceInfo(device.Handle);
        if (device_info.dwType <= RIM_TYPEHID && device_info.dwType >= 0)
        {
            // Get device parameters.
            USHORT usage_page = 0;
            USHORT usage = 0;
            DWORD flags = 0;
            switch (device_info.dwType)
            {
                case RIM_TYPEMOUSE:
                    usage_page = HID_USAGE_PAGE_GENERIC;
                    usage = HID_USAGE_GENERIC_MOUSE;
                    flags = RIDEV_INPUTSINK | RIDEV_NOLEGACY;
                    break;
                case RIM_TYPEKEYBOARD:
                    usage_page = HID_USAGE_PAGE_GENERIC;
                    usage = HID_USAGE_GENERIC_KEYBOARD;
                    flags = RIDEV_INPUTSINK | RIDEV_NOLEGACY;
                    break;
                case RIM_TYPEHID:
                    usage_page = device_info.hid.usUsagePage;
                    usage = device_info.hid.usUsage;
                    flags = RIDEV_INPUTSINK;
                    break;
            }
    
            // Pack parameters into tuple, validate inserts into set (non-duplicate).
            std::tuple<USHORT, USHORT, DWORD> parameters = std::make_tuple(usage_page, usage, flags);
            if (unique_parameters.insert(parameters).second)
            {
                // Create raw input device struct.
                RAWINPUTDEVICE raw_input_device = {};
                raw_input_device.usUsagePage = usage_page;
                raw_input_device.usUsage = usage;
                raw_input_device.dwFlags = flags;
                raw_input_device.hwndTarget = window_handle;
                raw_input_devices.push_back(raw_input_device);
            }
        }
    }

    // Register raw input devices.
    if (!raw_input_devices.empty())
    {
        if (RegisterRawInputDevices(raw_input_devices.data(), raw_input_devices.size(), sizeof(RAWINPUTDEVICE)) == TRUE)
        {
            return true;
        }
    }
    return false;
}

std::queue<InputEvent> WindowsRawInput::GetInputEvents()
{
    return std::queue<InputEvent>();
}

void WindowsRawInput::Initialize()
{
    // Initialize hidden window.
    HWND hidden_window_handle = InitializeHiddenWindow();
    if (hidden_window_handle == NULL)
    {
        std::cout << "Unable to initialize hidden window." << std::endl;
        return;
    }

    // Enumerate input devices.
    std::vector<WindowsRawInputDevice> devices = EnumerateDevices();

    // Register all input devices.
    if (!RegisterDevices(hidden_window_handle, devices))
    {
        std::cout << "Unable to register input devices." << std::endl;
    }

    // Message queue loop.
    while (ProcessMessageQueue()) {}
}

#endif