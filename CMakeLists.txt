# Configure C++ 23 project.
cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 23)
project(wobl CXX)

# Default to release config.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add include directories.
include_directories(
    include 
    include/stream
)

# Add source files to executable.
add_executable(wobl 
    src/main.cpp 
    src/Oscillator.cpp 
    src/Filter.cpp 
    src/Envelope.cpp 
    src/Wav.cpp 
    src/Noise.cpp
)

# Windows configuration.
if (WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
    target_link_libraries(wobl)
endif()
    
# Linux configuration.
if(NOT WIN32)
    add_definitions(-DPLATFORM_LINUX)
    target_link_libraries(wobl m)
endif()

# Compile options for release and debug configs.
target_compile_options(wobl PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native -ffast-math -fno-trapping-math>
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
)