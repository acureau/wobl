#pragma once

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <unordered_map>
#include "Input.hpp"
#include "InputDevice.hpp"
#include "InputDriver.hpp"

/*
    We should move the devices to their parent drivers. We can access them without recreating them after all.
    But then how will we determine which drivers are active? How will this interface take device IDs?
    Maybe we need a map of device ids to driver ids?

    That makes sense I guess, the only thing this class should manage is drivers. It should be a generic interface to those drivers,
    and invoke updates in them. It just needs to expose the means of browsing, selecting, and accessing the data of devices in a
    driver agnostic way.
*/

using StringPairVector = std::vector<std::pair<std::string, std::string>>;

class InputChangedCallback
{
    private:
        // some kind of variable for tracking last invocation time
    public:
        int UpdateIntervalMS;
        std::function<void(const StringPairVector &)> Callback;
};

class InputHandler
{
    private:
        // registered drivers map, id to object
        std::unordered_map<std::string, InputDriver> RegisteredDrivers;
        
        // active device ids list
        std::vector<std::string> ActiveDevices;

        // map of device ids to their containing driver ids
        std::unordered_map<std::string, std::string> DeviceDriverMap;

        // registered callbacks list
        std::vector<InputChangedCallback> RegisteredCallbacks;

        // invoke callback functions bound to input handler
        void InvokeCallbacks(const StringPairVector &updated_inputs);

        // main thread loop.
        void Update();

    public:
        // ID / name enumeration.
        StringPairVector EnumerateDevices();
        StringPairVector EnumerateInputs(std::string device_id);
        
        // Device activation / deactivation.
        void ActivateDevice(std::string device_id);
        void DeactivateDevice(std::string device_id);

        // Polling.
        template<typename T>
        T GetInputState(std::string device_id, std::string input_id);

        // Callback.
        void RegisterInputChangedCallback(int update_interval_ms, InputChangedCallback callback);

        // Handler initialization.
        void RegisterDriver(InputDriver driver);
        InputHandler();
};