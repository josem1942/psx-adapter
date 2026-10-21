cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_LIST_DIR}/boards)
set(PICO_BOARD psx_dev)

include(${CMAKE_CURRENT_LIST_DIR}/pico-sdk/pico_sdk_init.cmake)

project(psxc C CXX ASM)

pico_sdk_init()

add_executable(psxc src/main.c src/usb_descriptors.c src/psx.c )

pico_set_program_name(psxc "psxc")
pico_set_program_version(psxc "0.1")

target_link_libraries(psxc
        pico_stdlib
        tinyusb_device
        tinyusb_board)

target_include_directories(psxc PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
)

pico_add_extra_outputs(psxc)
