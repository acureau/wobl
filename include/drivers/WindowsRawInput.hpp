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
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <Windows.h>
#include <hidusage.h>
#include "InputDriver.hpp"
#include "InputEvent.hpp"

class WindowsRawInput : public InputDriver
{
    private:
        // The input event queue populated by the window proceedure.
        static std::queue<InputEvent> InputEventQueue;

        // Maps device handles to their ID and name.
        static std::unordered_map<HANDLE, std::pair<std::string, std::string>> DeviceHandleStringMap;
        
        // The window proceedure (windows input event callback handler).
        static LRESULT CALLBACK WindowProceedure(HWND window_handle, UINT message, WPARAM w_parameter, LPARAM l_parameter);
        
        // Processes a raw keyboard event.
        static void HandleRawKeyboardEvent(HANDLE device_handle, const RAWKEYBOARD &raw_keyboard_event);
        
        // Initializes a hidden window for input capture.
        HWND InitializeHiddenWindow();

        // Processes the window messages in the event queue.
        bool ProcessMessageQueue();

        // Gets the device ID (raw device path).
        std::string GetDeviceID(HANDLE device_handle);
        
        // Gets the device info struct.
        RID_DEVICE_INFO GetDeviceInfo(HANDLE device_handle);
        
        // Names the device something human-readable (poorly, as there are no good Windows APIs for this).
        std::string GetDeviceName(DWORD device_type, bool reset);
        
        // Determines whether a device is supported (yes, this struct is a single device).
        bool IsDeviceSupported(RAWINPUTDEVICELIST enumerated_device);
        
        // Enumerates all devices supported by the driver.
        std::vector<HANDLE> EnumerateDevices();

        // Constructs and applies an "event passing filter" supporting all passed devices.
        bool RegisterDevices(HWND window_handle, const std::vector<HANDLE>& devices);
        
    public:
        std::string Id = "Windows Raw Input Driver";
        void Initialize();
        std::queue<InputEvent> GetInputEvents();
};

#endif