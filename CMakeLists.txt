# Configure C++ 23 project.
cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 23)
project(wobl CXX)

# Default to release config.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add library.
add_library(wobl STATIC
    src/drivers/WindowsRawInput.cpp
    src/drivers/LinuxEvdev.cpp
    src/input/InputHandler.cpp
    src/output/Wav.cpp
    src/synthesis/Envelope.cpp
    src/synthesis/Filter.cpp
    src/synthesis/Noise.cpp
    src/synthesis/Oscillator.cpp
)

# Add library includes.
target_include_directories(wobl PUBLIC
    include/drivers
    include/input
    include/output
    include/synthesis
)

# Library compile options for release and debug targets.
target_compile_options(wobl PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native -ffast-math -fno-trapping-math>
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
)

# Windows configuration.
if (WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
endif()
    
# Linux configuration.
if(NOT WIN32)
    add_definitions(-DPLATFORM_LINUX)
    target_link_libraries(wobl m)
endif()

# TEST EXECUTABLES, TODO: REMOVE ME AND MAKE INDEPENDENT CMAKE PROJECTS

add_executable(synth-test
    tests/SynthTest.cpp
)
target_link_libraries(synth-test wobl)

add_executable(input-test
    tests/InputTest.cpp
)
target_link_libraries(input-test wobl)