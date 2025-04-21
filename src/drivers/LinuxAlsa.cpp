#include <iostream>
#include "LinuxAlsa.hpp"

// ALSA error handler that discards error messages, used to silence library output.
void AlsaNullErrorHandler(const char*, int, const char*, int, const char*, ...) {}

void LinuxAlsa::Initialize(std::shared_ptr<SampleCallback> sample_callback_pointer)
{
    // Set sample callback pointer.
    this->SampleCallbackPointer = sample_callback_pointer;

    // Silence ALSA library.
    snd_lib_error_set_handler(AlsaNullErrorHandler);
}

std::vector<OutputDevice> LinuxAlsa::EnumerateDevices()
{
    std::vector<OutputDevice> output_devices;

    // Get device hint buffer.
    void **device_hints;
    if (snd_device_name_hint(-1, "pcm", &device_hints) < 0 || !device_hints)
    {
        std::cerr << "ALSA: Unable to enumerate devices." << std::endl;
        return output_devices;
    }

    // Iterate over device hints.
    for (void** device_hint = device_hints; *device_hint != nullptr; device_hint++)
    {
        // Get device name.
        char* device_name = snd_device_name_get_hint(*device_hint, "NAME");
        if (device_name)
        {
            // Determine if device supports non-blocking PCM playback by trying to open a handle to it.
            snd_pcm_t* device_handle;
            if (snd_pcm_open(&device_handle, device_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) >= 0)
            {
                // Create output device.
                OutputDevice device;
                device.DriverId = this->Id;
                device.DeviceId = std::string(device_name);

                // Try to get human-readable name.
                char* device_description = snd_device_name_get_hint(*device_hint, "DESC");
                if (device_description)
                {
                    device.DeviceName = device_description;
                    free(device_description);
                }
                
                // Add output device to list.
                output_devices.push_back(device);

                // Close device handle.
                snd_pcm_close(device_handle);
            }
            free(device_name);
        }
    }

    // Free device hint buffer.
    snd_device_name_free_hint(device_hints);

    return output_devices;
}

OutputFormat LinuxAlsa::EnableDevice(const OutputDevice &device, const OutputFormat &requested_output_format)
{
    // There is no built in "negotiation". I need to determine the closest possible format manually. This is step one.
    // Step two is to pass the supported hwparams to the device.
    // Step three is to prepare it for flush calls.
    return OutputFormat {};
}

void LinuxAlsa::DisableDevice(const OutputDevice &device)
{
    // If device is active.
    if (this->DeviceIDHandleMap.contains(device.DeviceId))
    {
        // Retrieve its handle from the map, remove it.
        snd_pcm_t *device_handle = this->DeviceIDHandleMap[device.DeviceId];
        this->DeviceIDHandleMap.erase(device.DeviceId);
        
        // Close device handle.
        snd_pcm_close(device_handle);
    }
}

void LinuxAlsa::Flush()
{
    
}

LinuxAlsa::~LinuxAlsa()
{
    // Close all active device handles.
    for (auto& [device_id, device_handle] : this->DeviceIDHandleMap)
    {
        snd_pcm_close(device_handle);
    }
}

LinuxAlsa::LinuxAlsa()
{
    this->Id = "Linux ALSA Driver";
}