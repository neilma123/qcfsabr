cmake_minimum_required(VERSION 3.16)
project(statarb_sabr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(statarb
    src/main.cpp
    src/sabr.cpp
    src/backtest.cpp
    src/yahoo.cpp
)
target_include_directories(statarb PRIVATE src)

if(MSVC)
    target_compile_options(statarb PRIVATE /W4 /permissive-)
else()
    target_compile_options(statarb PRIVATE -Wall -Wextra -Wpedantic)
endif()

