cmake_minimum_required(VERSION 3.12)
project(multiplayer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(${ANDROID_ABI} STREQUAL "armeabi-v7a")
    add_definitions(-DVER_x32=true)
elseif(${ANDROID_ABI} STREQUAL "arm64-v8a")
    add_definitions(-DVER_x32=false)
endif()

# Add source files
file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/game/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/app/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/game/**/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/game/**/**/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/game/**/**/**/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/java_systems/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/java_systems/casino/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/util/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/gui/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/vendor/ini/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/vendor/RakNet/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/vendor/RakNet/SAMP/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/vendor/hash/*.cpp
        )

# Add include directories
include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui
        ${CMAKE_CURRENT_LIST_DIR}/vendor/
        ${CMAKE_CURRENT_LIST_DIR}/game/RW/
        ${CMAKE_CURRENT_LIST_DIR}/game/
        ${CMAKE_CURRENT_LIST_DIR}/java_systems
        ${CMAKE_CURRENT_LIST_DIR}/game/Core
)

add_library(multiplayer SHARED ${SOURCES})
target_link_libraries(multiplayer
        log
        GLESv3
        opus
        shadowhook::shadowhook
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/bass/libs/${ANDROID_ABI}/libbass.so
)

# Set compilation flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -w -fstack-protector -Wall -O3 -fexceptions") # old
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Og -fexceptions -pthread -g3 -gdwarf-5") # Debug

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -w -O3 -fexceptions -fstack-protector-strong -pthread")

if(${ANDROID_ABI} STREQUAL "arm64-v8a")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -mtune=cortex-a78")
elseif(${ANDROID_ABI} STREQUAL "armeabi-v7a")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfpu=neon -mfloat-abi=softfp")
endif()

if(${ANDROID_ABI} STREQUAL "arm64-v8a")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,max-page-size=65536")
endif()