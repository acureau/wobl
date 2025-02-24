cmake_minimum_required(VERSION 3.29)
project(wobl)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(include)
add_executable(wobl src/main.c src/oscillators.c)

target_compile_options(wobl PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native -ffast-math -fno-trapping-math>
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
)