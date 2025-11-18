cmake_minimum_required(VERSION 3.16)

project(FractalJulia)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 ")

find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

add_executable(fractal_julia main.cpp
fractal_serial.cpp palette.cpp fractal_simd.cpp
)

target_link_libraries(fractal_julia PRIVATE 
    fmt::fmt
    SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System)