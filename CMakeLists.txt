cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 23)
project(wobl CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(include)
add_executable(wobl src/main.cpp src/Oscillator.cpp src/Wav.cpp src/Noise.cpp)

# Link math library on linux.
if(NOT WIN32)
    target_link_libraries(wobl m)
endif()


target_compile_options(wobl PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native -ffast-math -fno-trapping-math>
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
)