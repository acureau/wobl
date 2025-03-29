/*
    Driver Type: Input
    Driver Target: Linux Input Subsystem Userspace API
*/

#ifdef PLATFORM_LINUX
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include "InputDriver.hpp"
#include "InputEvent.hpp"

// The number of bits in a ulong, used for device capability enumeration.
#define BITS_PER_LONG (sizeof(unsigned long) * 8)

// EV_KEY repeat and release codes.
#define KEY_REPEAT 2
#define KEY_RELEASE 0

// Internal driver device representation.
struct LinuxEvdevInputDevice
{
    int FileDescriptor;
    std::string ID;
    std::string Name;
};

class LinuxEvdev : public InputDriver
{
    private:
        // List of supported devices to poll.
        std::vector<LinuxEvdevInputDevice> Devices;

        // A map of key codes to their readable names.
        static const char* KeyCodeNameMap[KEY_CNT];

        // Populates the key code name map.
        void InitializeKeyCodeNameMap();

        // Converts a key code to a readable name string.
        std::string KeyCodeToString(unsigned short key_code);
        
        // Creates an InputEvent struct from an EV_KEY event.
        InputEvent ParseKeyEvent(const LinuxEvdevInputDevice &device, const input_event &key_event);

        // Determines if a given device has keyboard capabilities.
        bool IsKeyboard(LinuxEvdevInputDevice device);

        // Determines if a given device is supported by the driver.
        bool IsDeviceSupported(LinuxEvdevInputDevice device);

        // Enumerates supported drivers, populating Devices list.
        void EnumerateDevices();

    public:
        std::string Id = "Linux Evdev Driver";
        void Initialize();
        std::queue<InputEvent> GetInputEvents();
        ~LinuxEvdev();
};

#endif