cmake_minimum_required(VERSION 3.7)

include(/Users/veegee/lib/pico-sdk/pico_sdk_init.cmake)

project(main C CXX ASM)

pico_sdk_init()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*.*")

add_executable(${PROJECT_NAME} ${SOURCES})

# pull in common dependencies
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_i2c hardware_spi hardware_pwm hardware_pio hardware_dma)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})
