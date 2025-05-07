#include <iostream>
#include <sstream>
#include <cstdio>
#include "LinuxAlsa.hpp"

// ALSA error handler that discards error messages, used to silence library output.
void AlsaNullErrorHandler(const char*, int, const char*, int, const char*, ...) {}

const std::vector<std::pair<std::string, snd_pcm_format_t>> LinuxAlsa::SampleFormatEnumPairs = {
    {"S_I_8", SND_PCM_FORMAT_S8},
    {"U_I_8", SND_PCM_FORMAT_U8},
    {"S_I_16", SND_PCM_FORMAT_S16_LE},
    {"U_I_16", SND_PCM_FORMAT_U16_LE},
    {"S_I_24", SND_PCM_FORMAT_S24_3LE},
    {"U_I_24", SND_PCM_FORMAT_U24_3LE},
    {"S_I_32", SND_PCM_FORMAT_S32_LE},
    {"U_I_32", SND_PCM_FORMAT_U32_LE},
    {"S_F_32", SND_PCM_FORMAT_FLOAT_LE}
};

std::string LinuxAlsa::OutputFormatToEnumKey(const OutputFormat &output_format)
{
    std::ostringstream enum_key_stream;
    enum_key_stream 
        << (output_format.Sign == Signed ? "S" : "U") << '_' 
        << (output_format.Type == FloatingPoint ? "F" : "I") << '_' 
        << output_format.BitsPerSample; 
    return enum_key_stream.str();
}

OutputFormat LinuxAlsa::FormatEnumKeyToOutputFormat(const std::string &format_enum_key)
{
    OutputFormat output_format;
    char sign_char, type_char;
    int bits;
    
    // Parse string components.
    if (sscanf(format_enum_key.c_str(), "%c_%c_%d", &sign_char, &type_char, &bits) != 3) {
        std::cerr << "ALSA: Unable to convert enum key to output format." << std::endl;
        return {};
    }

    // Populate relevant output format properties.
    output_format.Sign = (sign_char == 'S') ? Signed : Unsigned;
    output_format.Type = (type_char == 'F') ? FloatingPoint : Integer;
    output_format.BitsPerSample = bits;

    return output_format;
}

OutputFormat LinuxAlsa::FormatEnumToOutputFormat(snd_pcm_format_t format_enum)
{
    // Find the corresponding key string for the enum.
    for (const auto& pair : SampleFormatEnumPairs)
    {
        if (pair.second == format_enum)
        {
            // Convert the key to an internal output format object.
            return FormatEnumKeyToOutputFormat(pair.first);
        }
    }
    return {};
}

std::vector<snd_pcm_format_t> LinuxAlsa::GetOrderedFormatNegotiationEnums(const OutputFormat &output_format)
{
    // Get output sample format ALSA enum key.
    std::string format_enum_key = OutputFormatToEnumKey(output_format);

    // Find index of enum key.
    int enum_key_index = -1;
    for (int i = 0; i < SampleFormatEnumPairs.size(); ++i) 
    {
        if (SampleFormatEnumPairs[i].first == format_enum_key) 
        {
            enum_key_index = i;
            break;
        }
    }

    if (enum_key_index == -1)
    {
        std::cerr << "ALSA: Unable to find index of enum key in list." << std::endl;
        return {};
    }

    // Build an ordered list of enums to try in negotiation.
    std::vector<snd_pcm_format_t> negotiation_enums { SampleFormatEnumPairs[enum_key_index].second };
    int offset = 1;
    bool overflow = false;
    bool underflow = false;
    while (!overflow || !underflow)
    {
        // Check positive offset.
        if (!overflow && (enum_key_index + offset <= SampleFormatEnumPairs.size() - 1))
        {
            negotiation_enums.push_back(SampleFormatEnumPairs[enum_key_index + offset].second);
        }
        else 
        {
            overflow = true;
        }

        // Check negative offset.
        if (!underflow && (enum_key_index - offset >= 0))
        {
            negotiation_enums.push_back(SampleFormatEnumPairs[enum_key_index - offset].second);
        }
        else 
        {
            underflow = true;
        }

        // Increment offset.
        offset++;
    }

    return negotiation_enums;
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
        snd_pcm_close(device_handle);
        return {};
    }

    // Allocate hw_params structure.
    snd_pcm_hw_params_t* hw_params;
    if (snd_pcm_hw_params_malloc(&hw_params) < 0)
    {
        std::cerr << "ALSA: Unable to allocate hw_params structure." << std::endl;
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(device_handle);
        return {};
    }
    
    // Get the supported range of configurations.
    if (snd_pcm_hw_params_any(device_handle, hw_params) < 0)
    {
        std::cerr << "ALSA: Unable to populate hw_params structure." << std::endl;
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(device_handle);
        return {};
    }

    // Set access type.
    if (snd_pcm_hw_params_set_access(device_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
    {
        std::cerr << "ALSA: Unable to set access type." << std::endl;
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(device_handle);
        return {};
    }

    // Negotiate sample rate.
    unsigned int negotiated_sample_rate = requested_output_format.SampleRate;
    if (snd_pcm_hw_params_set_rate_near(device_handle, hw_params, &negotiated_sample_rate, 0) < 0)
    {
        std::cerr << "ALSA: Unable to negotiate sample rate." << std::endl;
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(device_handle);
        return {};
    }
    device_format.SampleRate = negotiated_sample_rate;

    // Negotiate channel count.
    unsigned int negotiated_channel_count = requested_output_format.ChannelCount;
    if (snd_pcm_hw_params_set_channels_near(device_handle, hw_params, &negotiated_channel_count) < 0)
    {
        std::cerr << "ALSA: Unable to negotiate channel count." << std::endl;
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(device_handle);
        return {};
    }
    device_format.ChannelCount = negotiated_channel_count;

    // Negotiate sample format.
    snd_pcm_format_t negotiated_format_enum = SND_PCM_FORMAT_UNKNOWN;
    for (snd_pcm_format_t format_enum : GetOrderedFormatNegotiationEnums(requested_output_format))
    {
        if (snd_pcm_hw_params_test_format(device_handle, hw_params, format_enum) == 0)
        {
            negotiated_format_enum = format_enum;
            break;
        }
    }
    
    // Set sample format in hw_params and returned output format.
    if (negotiated_format_enum == SND_PCM_FORMAT_UNKNOWN)
    {
        std::cerr << "ALSA: Unable to negotiate sample format." << std::endl;
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(device_handle);
        return {};
    }
    else
    {
        if (snd_pcm_hw_params_set_format(device_handle, hw_params, negotiated_format_enum) < 0)
        {
            std::cerr << "ALSA: Unable to set sample format." << std::endl;
            snd_pcm_hw_params_free(hw_params);
            snd_pcm_close(device_handle);
            return {};
        }

        OutputFormat negotiated_output_sample_format = FormatEnumToOutputFormat(negotiated_format_enum);
        if (negotiated_output_sample_format.BitsPerSample == 0)
        {
            std::cerr << "ALSA: Unable to find enum in known format pairs." << std::endl;
            snd_pcm_hw_params_free(hw_params);
            snd_pcm_close(device_handle);
            return {};
        }
        else 
        {
            device_format.BitsPerSample = negotiated_output_sample_format.BitsPerSample;
            device_format.Sign = negotiated_output_sample_format.Sign;
            device_format.Type = negotiated_output_sample_format.Type;
        }
    }

    // Pass hw_params to the device, free structure.
    if (snd_pcm_hw_params(device_handle, hw_params) < 0)
    {
        std::cerr << "ALSA: Unable to set device hardware parameters." << std::endl;
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(device_handle);
        return {};
    }
    snd_pcm_hw_params_free(hw_params);
    
    // Add device handle to id map.
    this->DeviceIDHandleMap[device.DeviceId] = device_handle;

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
    // Iterate over all active devices.
    for (auto& [device_id, device_handle] : this->DeviceIDHandleMap)
    {
        // Get number of frames available to write.
        snd_pcm_sframes_t available_frames = snd_pcm_avail_update(device_handle);
        if (available_frames < 0) {
            std::cerr << "ALSA: Failed to get available frame count for device '" << device_id << "'." << std::endl;
            continue;
        }

        // Continue if no frames available.
        if (available_frames == 0) continue;
        
        // Build samples vector.
        std::vector<std::byte> samples;
        for (snd_pcm_sframes_t frame_count = 0; frame_count < available_frames; frame_count++)
        {
            std::vector<std::byte> sample_frame = (*this->SampleCallbackPointer)(this->Id, device_id);
            samples.insert(samples.end(), sample_frame.begin(), sample_frame.end());
        }

        // Write samples vector to device.
        snd_pcm_sframes_t result = snd_pcm_writei(device_handle, samples.data(), available_frames);
        if (result < 0)
        {
            // Recover from buffer underrun.
            if (result == -EPIPE)
            {
                snd_pcm_prepare(device_handle);
            }
            else
            {
                std::cerr << "ALSA: Unable to write frames for device '" << device_id << "'." << std::endl;
            }
        }
    }
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