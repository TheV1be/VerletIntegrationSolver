cmake_minimum_required(VERSION 3.16)
project(VerletSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release if no build type is specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# Automatically collect all .cpp files inside src/
# CONFIGURE_DEPENDS to auto-reconfigure when add/delete files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(sim ${SOURCES})

# Allow #include "Particle.hpp" from anywhere without relative paths like "../include/"
target_include_directories(sim PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Enable compiler warnings
target_compile_options(sim PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(sim PRIVATE sfml-graphics sfml-window sfml-system)

