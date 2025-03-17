#pragma once

#include <vector>
#include <string>
#include <functional>
#include <optional>

class DeviceIdentity
{
    public:
        std::string Identifier; // A unique identifier for the device. 
        std::string Name;       // A human-readable name for the device

        DeviceIdentity(std::string identifier, std::string name):
            Identifier(identifier), Name(name) {}
};

class DeviceParams
{
    public:
        std::optional<int> SampleRate;     // The sample rate supported by the device.
        std::optional<int> ChannelCount;   // The channel count supported by the device.
};

class Stream
{
    public:
        // Gets a list of devices.
        virtual std::vector<DeviceIdentity> GetDevices() = 0;

        // Sets the active device.
        virtual bool SetDevice(DeviceIdentity device) = 0;

        // Initializes playback, returning device capabilities.
        virtual bool Initialize() = 0;

        // Start playback.
        virtual bool Start(std::function<std::vector<float>(DeviceParams, int)> callback) = 0;

        // Stop playback.
        virtual bool Stop() = 0;

        virtual ~Stream() = default;
};