cmake_minimum_required(VERSION 3.7)

include(/home/veegee/pico/pico-sdk/pico_sdk_init.cmake)

project(main C CXX ASM)

pico_sdk_init()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*.*")

add_executable(${PROJECT_NAME} ${SOURCES})

# pull in common dependencies
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_i2c hardware_spi hardware_pwm hardware_pio hardware_dma)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})
