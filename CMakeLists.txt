# Configure C++ 23 project.
cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 23)
project(wobl CXX)

# Default to release config.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add library.
add_library(libwobl STATIC
    src/synthesis/Envelope.cpp
    src/synthesis/Filter.cpp
    src/synthesis/Noise.cpp
    src/synthesis/Oscillator.cpp
    src/synthesis/Wav.cpp
)

# Add library includes.
target_include_directories(libwobl PUBLIC
    include/input
    include/output
    include/synthesis
)

# Library compile options for release and debug targets.
target_compile_options(libwobl PRIVATE
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
    target_link_libraries(libwobl m)
endif()

# TEST EXECUTABLE, TODO: REMOVE ME AND MAKE AN INDEPENDENT CMAKE PROJECT

add_executable(test
    tests/main.cpp
)

target_link_libraries(test libwobl)