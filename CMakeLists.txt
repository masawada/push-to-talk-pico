cmake_minimum_required(VERSION 3.17)

if (NOT PICO_SDK_PATH AND NOT DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/pico-sdk)
endif()

include(pico_sdk_import.cmake)
project(push_to_talk_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(${PROJECT_NAME} src/main.c src/usb_descriptors.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(${PROJECT_NAME} PUBLIC
    pico_stdlib pico_unique_id tinyusb_device tinyusb_board)
pico_add_extra_outputs(${PROJECT_NAME})
