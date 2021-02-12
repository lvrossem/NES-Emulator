cmake_minimum_required(VERSION 3.19.3)
project(NES_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set(SOURCE_FILES src/main.cpp src/nes.h src/nes.cpp src/cpu.h src/cpu.cpp src/cpu_memory.h src/cpu_memory.cpp src/ppu.h src/ppu.cpp)
add_executable(NES ${SOURCE_FILES})

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(NES ${SDL2_LIBRARIES})