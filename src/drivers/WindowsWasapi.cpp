#include <iostream>
#include <combaseapi.h>
#include <propkeydef.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "WindowsWasapi.hpp"

std::string WindowsWasapi::WideStringToString(LPWSTR lp_wide_string)
{
    // Convert to a standard wide string, then to a standard string.
    std::wstring wide_string(lp_wide_string);
    std::string string(wide_string.begin(), wide_string.end());
    return string;
}

std::string WindowsWasapi::GetDeviceID(IMMDevice* device)
{
    std::string device_id = "";
    
    LPWSTR wide_device_id;
    if(device->GetId(&wide_device_id) != S_OK)
    {
        std::cerr << "WASAPI: Failed to get device identifier." << std::endl;
    }
    else 
    {
        device_id = WideStringToString(wide_device_id);
    }
    CoTaskMemFree(wide_device_id);
    
    return device_id;
}

std::string WindowsWasapi::GetDeviceName(IMMDevice* device)
{
    std::string device_name = "";

    IPropertyStore* property_store;
    if (device->OpenPropertyStore(STGM_READ, &property_store) != S_OK)
    {
        std::cerr << "WASAPI: Failed to get device property store." << std::endl;
    }
    else
    {
        PROPVARIANT device_name_property;
        if (property_store->GetValue(PKEY_Device_FriendlyName, &device_name_property) != S_OK)
        {
            std::cerr << "WASAPI: Failed to get device name property." << std::endl;
        }
        else
        {
            device_name = WideStringToString(device_name_property.pwszVal);
        }
        PropVariantClear(&device_name_property);
    }
    property_store->Release();
    
    return device_name;
}

WAVEFORMATEXTENSIBLE WindowsWasapi::OutputFormatToWaveFormatExtensible(OutputFormat output_format)
{
    WAVEFORMATEXTENSIBLE wave_format = {};
    wave_format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wave_format.Format.nChannels = static_cast<WORD>(output_format.ChannelCount);
    wave_format.Format.nSamplesPerSec = static_cast<DWORD>(output_format.SampleRate);
    wave_format.Format.wBitsPerSample = static_cast<WORD>(output_format.BitsPerSample);
    wave_format.Format.nBlockAlign = static_cast<WORD>((output_format.BitsPerSample / 8) * output_format.ChannelCount);
    wave_format.Format.nAvgBytesPerSec = wave_format.Format.nSamplesPerSec * wave_format.Format.nBlockAlign;
    wave_format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
    wave_format.Samples.wValidBitsPerSample = static_cast<WORD>(output_format.BitsPerSample);

    if (output_format.Type == SampleType::FloatingPoint)
    {
        wave_format.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    }
    else
    {
        wave_format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    }

    return wave_format;
}

OutputFormat WindowsWasapi::WaveFormatToOutputFormat(WAVEFORMATEX* wave_format)
{
    OutputFormat format = {};
    format.SampleRate = static_cast<int>(wave_format->nSamplesPerSec);
    format.BitsPerSample = static_cast<int>(wave_format->wBitsPerSample);
    format.ChannelCount = static_cast<int>(wave_format->nChannels);
    format.Type = Integer;
    format.Sign = Signed;

    if (wave_format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
    {
        format.Type = FloatingPoint;
    }
    else if (wave_format->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        WAVEFORMATEXTENSIBLE* ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(wave_format);
        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
        {
            format.Type = FloatingPoint;
        }
    }

    return format;
}

void WindowsWasapi::FreeWasapiOutputDevice(WasapiOutputDevice& internal_device)
{
    if (internal_device.AudioClient)
    {
        internal_device.AudioClient->Stop();
        internal_device.AudioClient->Release();
    }
    if (internal_device.RenderClient) internal_device.RenderClient->Release();
    if (internal_device.RenderEvent) CloseHandle(internal_device.RenderEvent);
}

void WindowsWasapi::Initialize() 
{
    // Initialize the COM library.
    if (CoInitialize(NULL) != S_OK) 
    {
        std::cerr << "WASAPI: Failed to initialize COM library." << std::endl;
    }

    // Initialize MMDeviceEnumerator.
    HRESULT result = CoCreateInstance
    (
        __uuidof(MMDeviceEnumerator),
        NULL,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (LPVOID*)&DeviceEnumerator
    );

    if (result != S_OK) 
    {
        std::cerr << "WASAPI: Failed to create device enumerator." << std::endl;
    }
}

std::vector<OutputDevice> WindowsWasapi::EnumerateDevices() 
{
    std::vector<OutputDevice> devices;

    // Reset enumeration state.
    this->DeviceIDPointerMap.clear();

    // Ensure enumerator is initialized.
    if (!DeviceEnumerator)
    {
        std::cerr << "WASAPI: Device enumerator not initialized, could not enumerate devices." << std::endl;
        return devices;
    }

    // Enumerate active output devices.
    IMMDeviceCollection* device_collection;
    if (DeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &device_collection) != S_OK)
    {
        std::cerr << "WASAPI: Failed to get device collection." << std::endl;
        return devices;
    }
    
    // Count devices.
    UINT device_count;
    if (device_collection->GetCount(&device_count) != S_OK)
    {
        std::cerr << "WASAPI: Failed to get device count." << std::endl;
        return devices;
    }

    // Iterate over all devices, adding them to the vector and id / pointer map.
    for (UINT device_index = 0; device_index < device_count; device_index++)
    {
        IMMDevice* device;
        if (device_collection->Item(device_index, &device) != S_OK)
        {
            std::cerr << "WASAPI: Failed to get device at index '" << device_index << "'." << std::endl;
            continue;
        }
        else
        {
            // Get device.
            std::string device_id = GetDeviceID(device);

            // Construct and insert output device object.
            devices.emplace_back(this->Id, device_id, GetDeviceName(device));

            // Add id / pointer pair to map.
            this->DeviceIDPointerMap[device_id] = device;
        }
    }

    // Release device collection.
    device_collection->Release();

    return devices;
}

OutputFormat WindowsWasapi::EnableDevice(const OutputDevice &device, const OutputFormat &requested_output_format) 
{
    // Create internal output device structure.
    WasapiOutputDevice* internal_device_pointer = new WasapiOutputDevice{};
    OutputFormat device_format = requested_output_format;
    
    // Retrieve the device pointer.
    if (!this->DeviceIDPointerMap.contains(device.DeviceId))
    {
        std::cerr << "WASAPI: Cannot find pointer to device '" << device.DeviceId << "'." << std::endl;
        FreeWasapiOutputDevice(*internal_device_pointer);
        return {};
    }
    IMMDevice* device_pointer = this->DeviceIDPointerMap[device.DeviceId];

    // Create audio client.
    HRESULT result = device_pointer->Activate
    (
        __uuidof(IAudioClient),
        CLSCTX_ALL,
        NULL,
        (void**)&(*internal_device_pointer).AudioClient
    );

    if (result != S_OK)
    {
        std::cerr << "WASAPI: Failed to create audio client." << std::endl;
        FreeWasapiOutputDevice(*internal_device_pointer);
        return {};
    }

    // Negotiate output format.
    WAVEFORMATEXTENSIBLE requested_wave_format = OutputFormatToWaveFormatExtensible(requested_output_format);
    WAVEFORMATEX* closest_wave_format;
    result = (*internal_device_pointer).AudioClient->IsFormatSupported(
        AUDCLNT_SHAREMODE_SHARED,
        reinterpret_cast<WAVEFORMATEX*>(&requested_wave_format),
        &closest_wave_format
    );

    // If no closest format found, get the supported format.
    if (result != S_OK && !closest_wave_format)
    {
        if ((*internal_device_pointer).AudioClient->GetMixFormat(&closest_wave_format) != S_OK)
        {
            std::cerr << "WASAPI: Failed to determine a supported wave format." << std::endl;
            FreeWasapiOutputDevice(*internal_device_pointer);
            return {};
        }
    }

    // Get device period.
    REFERENCE_TIME device_period;
    if ((*internal_device_pointer).AudioClient->GetDevicePeriod(NULL, &device_period) != S_OK)
    {
        std::cerr << "WASAPI: Failed to retrieve device period." << std::endl;
        FreeWasapiOutputDevice(*internal_device_pointer);
        return {};
    }

    // Initialize audio client.
    result = (*internal_device_pointer).AudioClient->Initialize
    (
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        device_period,
        0,
        closest_wave_format ? closest_wave_format : reinterpret_cast<WAVEFORMATEX*>(&requested_wave_format),
        NULL
    );

    if (result != S_OK)
    {
        std::cerr << "WASAPI: Failed to initialize audio client." << std::endl;
        FreeWasapiOutputDevice(*internal_device_pointer);
        return {};
    }

    // Start audio client.
    (*internal_device_pointer).AudioClient->Start();

    // Free negotiated format if requested format was not supported.
    if (closest_wave_format) 
    {
        device_format = WaveFormatToOutputFormat(closest_wave_format);
        CoTaskMemFree(closest_wave_format);
    }

    // // Initialize event handler.
    (*internal_device_pointer).RenderEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if ((*internal_device_pointer).RenderEvent == NULL)
    {
        std::cerr << "WASAPI: Failed to create render event handle." << std::endl;
        FreeWasapiOutputDevice(*internal_device_pointer);
        return {};
    }

    if ((*internal_device_pointer).AudioClient->SetEventHandle((*internal_device_pointer).RenderEvent) != S_OK)
    {
        std::cerr << "WASAPI: Failed to set render event handle." << std::endl;
        FreeWasapiOutputDevice(*internal_device_pointer);
        return {};
    }

    // Initialize render client.
    result = (*internal_device_pointer).AudioClient->GetService
    (
        __uuidof(IAudioRenderClient),
        (void**)&(*internal_device_pointer).RenderClient
    );

    if (result != S_OK)
    {
        std::cerr << "WASAPI: Failed to create render client." << std::endl;
        FreeWasapiOutputDevice(*internal_device_pointer);
        return {};
    }

    // Add internal device structure to id / internal device map.
    this->DeviceIDInternalDeviceMap[device.DeviceId] = internal_device_pointer;

    return device_format;
}

void WindowsWasapi::DisableDevice(const OutputDevice &device) 
{
    // If device is active, free memory it points at and remove from map.
    if (this->DeviceIDInternalDeviceMap.contains(device.DeviceId))
    {
        FreeWasapiOutputDevice(*DeviceIDInternalDeviceMap[device.DeviceId]);
        this->DeviceIDInternalDeviceMap.erase(device.DeviceId);
    }
}

/*
    Well trying to implement this just exposed a significant flaw in how I'm trying to receive samples. 
    The driver shouldn't be fed samples itself to buffer, I already have an output handler with a ring buffer. 
    It needs to be passed a callback to invoke each time it wants a sample frame.

    We already return an output format to the output handler. It needs to store this information with the 
    driver ID and device ID strings. We need to identify which device is the caller in the callback function,
    so we need to pass the driver ID and device ID to retrieve a sample from the ring buffer.

    Both of these fields are stored in the device object already. I don't want to extend it though, since
    it's passed to the client. We'll have to store its negotiated output format and buffer index in some
    internal output handler struct.
*/

void WindowsWasapi::Flush() 
{
    
}

WindowsWasapi::~WindowsWasapi() 
{
    // Release device enumerator.
    if (DeviceEnumerator) DeviceEnumerator->Release();
    
    // Release device pointers.
    for (auto& [device_id, device_pointer] : this->DeviceIDPointerMap) 
    {
        device_pointer->Release();
    }

    // Release active device structures.
    for (auto& [device_id, internal_device_pointer] : this->DeviceIDInternalDeviceMap)
    {
        FreeWasapiOutputDevice(*internal_device_pointer);
    }

    // Uninitialize the COM library.
    CoUninitialize();
}
