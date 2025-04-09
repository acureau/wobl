#pragma once

#include <mutex>
#include <string>
#include <vector>
#include <cstddef>
#include <unordered_map>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include "OutputFormat.hpp"
#include "OutputDevice.hpp"
#include "OutputDriver.hpp"

struct WasapiOutputDevice
{
    IAudioClient* AudioClient;
    IAudioRenderClient* RenderClient;
    HANDLE RenderEvent;
    std::vector<std::byte> Buffer;
    std::mutex BufferMutex;
};

class WindowsWasapi : public OutputDriver
{
    private:
        IMMDeviceEnumerator* DeviceEnumerator = nullptr;
        std::unordered_map<std::string, IMMDevice*> DeviceIDPointerMap;
        std::unordered_map<std::string, WasapiOutputDevice*> DeviceIDInternalDeviceMap;

        static std::string WideStringToString(LPWSTR lp_wide_string);
        static std::string GetDeviceID(IMMDevice* device);
        static std::string GetDeviceName(IMMDevice* device);
        static WAVEFORMATEXTENSIBLE OutputFormatToWaveFormatExtensible(OutputFormat output_format);
        static OutputFormat WaveFormatToOutputFormat(WAVEFORMATEX* wave_format);
        static void FreeWasapiOutputDevice(WasapiOutputDevice& internal_device);

    public:
        std::string Id = "Windows WASAPI Driver";
        void Initialize() override;
        std::vector<OutputDevice> EnumerateDevices() override;
        OutputFormat EnableDevice(const OutputDevice &device, const OutputFormat &requested_output_format) override;
        void DisableDevice(const OutputDevice &device) override;
        void Flush() override;
        ~WindowsWasapi() override;
};
