#ifdef PLATFORM_WINDOWS

#include "WindowsRawInput.hpp"

int main()
{
    WindowsRawInput driver;
    driver.Initialize();
    return 0;
}

#endif

#ifdef PLATFORM_LINUX

#include "LinuxEvdev.hpp"

int main()
{
    LinuxEvdev driver;
    driver.Initialize();
    return 0;
}

#endif