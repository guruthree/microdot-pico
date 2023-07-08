cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD "pico_w" CACHE STRING "" FORCE)

include(pico_sdk_import.cmake)

project(microdot-pico_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(microdot-pico
    microdot-pico.cpp
    MicroMatrix.cpp
    IS31FL3730/IS31FL3730.cpp
    Adafruit_GFX_Library/Adafruit_GFX.cpp
)

target_include_directories(microdot-pico PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(microdot-pico
    pico_stdlib
    hardware_i2c
    pico_malloc
    pico_mem_ops
)
pico_add_extra_outputs(microdot-pico)


pico_enable_stdio_usb(microdot-pico 1)
pico_enable_stdio_uart(microdot-pico 0)

