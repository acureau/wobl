#include "InputHandler.hpp"

#ifdef PLATFORM_WINDOWS
#include "WindowsRawInput.hpp"
#endif

#ifdef PLATFORM_LINUX
#include "LinuxEvdev.hpp"
#endif 

void InputHandler::Update()
{
    // Poll all drivers, combine retrieved input events.
    std::queue<InputEvent> all_input_events;
    {
        // Acquire driver list lock.
        std::lock_guard<std::mutex> driver_lock(this->DriverMutex);
        for (std::unique_ptr<InputDriver> &driver : this->RegisteredInputDrivers)
        {
            std::queue<InputEvent> driver_input_events = driver->GetInputEvents();
            while (!driver_input_events.empty())
            {
                all_input_events.push(driver_input_events.front());
                driver_input_events.pop();
            }
        }
    }
    
    // Fire callback functions if any input event retrieved.
    if (!all_input_events.empty())
    {
        // Acquire callback list lock.
        std::lock_guard<std::mutex> callback_lock(this->CallbackMutex);

        // Fire all callbacks.
        for (std::shared_ptr<InputEventCallback> &callback : this->RegisteredCallbacks)
        {
            (*callback)(all_input_events);
        }
    }
}

void InputHandler::RegisterDriver(std::unique_ptr<InputDriver> driver)
{
    // Acquire driver list lock.
    std::lock_guard<std::mutex> driver_lock(this->DriverMutex);

    // Initialize driver and add to the active driver list.
    driver->Initialize();
    RegisteredInputDrivers.push_back(std::move(driver));
}

void InputHandler::RegisterInputEventCallback(std::shared_ptr<InputEventCallback> callback)
{
    // Acquire callback list lock.
    std::lock_guard<std::mutex> callback_lock(this->CallbackMutex);

    // Add callback to list.
    this->RegisteredCallbacks.push_back(callback);
}

InputHandler::InputHandler()
{
    // Register platform specific drivers.
    #ifdef PLATFORM_WINDOWS
    this->RegisterDriver(std::make_unique<WindowsRawInput>());
    #endif 

    #ifdef PLATFORM_LINUX
    this->RegisterDriver(std::make_unique<LinuxEvdev>());
    #endif 

    // Spawn polling thread.
    this->PollingThreadActive = true;
    this->PollingThread = std::thread([this]() {
        while (this->PollingThreadActive) {
            this->Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });
}

InputHandler::~InputHandler()
{
    // Clean up polling thread.
    this->PollingThreadActive = false;
    if (this->PollingThread.joinable())
    {
        this->PollingThread.join();
    }
}