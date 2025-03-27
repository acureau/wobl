#ifdef PLATFORM_WINDOWS

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>

#include "Windows.h"
#include "hidusage.h"

#include "WindowsRawInput.hpp"

// todo: replace with code for generating input events
void ProcessRawInput(HRAWINPUT hRawInput)
{
    UINT dwSize = 0;

    // Get required size
    GetRawInputData(hRawInput, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
    std::vector<BYTE> lpb(dwSize);

    if (GetRawInputData(hRawInput, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
    {
        std::cerr << "GetRawInputData didn't return correct size.\n";
        return;
    }

    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.data());

    // Identify device
    HANDLE hDevice = raw->header.hDevice;

    switch (raw->header.dwType)
    {
        case RIM_TYPEKEYBOARD:
        {
            const RAWKEYBOARD& kb = raw->data.keyboard;

            std::cout << "[Keyboard] Device: " << hDevice
                      << ", VKey: " << kb.VKey
                      << ", Flags: " << kb.Flags
                      << ", Message: " << kb.Message
                      << "\n";
            break;
        }
        case RIM_TYPEMOUSE:
        {
            const RAWMOUSE& mouse = raw->data.mouse;

            std::cout << "[Mouse] Device: " << hDevice
                      << ", Buttons: " << mouse.usButtonFlags
                      << ", X: " << mouse.lLastX
                      << ", Y: " << mouse.lLastY
                      << "\n";
            break;
        }
        case RIM_TYPEHID:
        {
            const RAWHID& hid = raw->data.hid;

            std::cout << "[HID] Device: " << hDevice
                      << ", ReportID: " << static_cast<int>(hid.bRawData[0])
                      << ", Size: " << hid.dwSizeHid
                      << ", Count: " << hid.dwCount
                      << "\n";

            // For raw HID data parsing, you'll need the report descriptor
            // from HID APIs like HidP_GetUsages(), etc.

            break;
        }
    }
}

LRESULT CALLBACK WindowProceedure(HWND window_handle, UINT message, WPARAM w_parameter, LPARAM l_parameter) {
    // Handle WM_INPUT events, fired by the Raw Input API.
    if (message == WM_INPUT)
    {
        ProcessRawInput(reinterpret_cast<HRAWINPUT>(l_parameter));
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

struct Device
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

std::vector<Device> EnumerateDevices()
{
    std::vector<Device> devices;

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

    return devices;
}

bool RegisterDevices(HWND window_handle, const std::vector<Device>& devices)
{
    // Store unique sets of parameters, created raw input device structs.
    std::set<std::tuple<USHORT, USHORT, DWORD>> unique_parameters;
    std::vector<RAWINPUTDEVICE> raw_input_devices;

    // Iterate over devices list.
    for (Device device : devices)
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
    std::vector<Device> devices = EnumerateDevices();

    // Register all input devices.
    if (!RegisterDevices(hidden_window_handle, devices))
    {
        std::cout << "Unable to register input devices." << std::endl;
    }

    // Message queue loop.
    while (ProcessMessageQueue()) {}
}

#endif