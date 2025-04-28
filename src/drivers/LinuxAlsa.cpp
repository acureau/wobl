#include <iostream>
#include "LinuxAlsa.hpp"

// ALSA error handler that discards error messages, used to silence library output.
void AlsaNullErrorHandler(const char*, int, const char*, int, const char*, ...) {}

OutputFormat LinuxAlsa::ConvertAlsaToInternalSampleFormat(snd_pcm_format_t alsa_sample_format)
{

}

snd_pcm_format_t LinuxAlsa::ConvertInternalToAlsaSampleFormat(OutputFormat internal_sample_format)
{

}

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
    OutputFormat device_format;

    // Open device handle.
    snd_pcm_t* device_handle;
    if (snd_pcm_open(&device_handle, device.DeviceId.c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) < 0)
    {
        std::cerr << "ALSA: Unable to open device handle." << std::endl;
        return {};
    }

    // Allocate hw_params structure.
    snd_pcm_hw_params_t* hw_params;
    if (snd_pcm_hw_params_malloc(&hw_params) < 0)
    {
        std::cerr << "ALSA: Unable to allocate hw_params structure." << std::endl;
        return {};
    }
    
    // Get the supported range of configurations.
    if (snd_pcm_hw_params_any(device_handle, hw_params) < 0)
    {
        std::cerr << "ALSA: Unable to populate hw_params structure." << std::endl;
        return {};
    }

    // Negotiate sample rate.
    unsigned int negotiated_sample_rate = requested_output_format.SampleRate;
    if (snd_pcm_hw_params_set_rate_near(device_handle, hw_params, &negotiated_sample_rate, 0) < 0)
    {
        std::cerr << "ALSA: Unable to negotiate sample rate." << std::endl;
        return {};
    }
    device_format.SampleRate = negotiated_sample_rate;

    // Negotiate channel count.
    unsigned int negotiated_channel_count = requested_output_format.ChannelCount;
    if (snd_pcm_hw_params_set_channels_near(device_handle, hw_params, &negotiated_channel_count) < 0)
    {
        std::cerr << "ALSA: Unable to negotiate channel count." << std::endl;
        return {};
    }
    device_format.ChannelCount = negotiated_channel_count;

    // Negotiate sample format.

    // Step two is to pass the supported hwparams to the device.
    // Step three is to prepare it for flush calls.
    return device_format;
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