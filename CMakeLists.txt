cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_HOME}/pico_sdk_init.cmake)

project(robot)

pico_sdk_init()

add_executable(robot 
    src/main.c
    src/spi.c
    src/motors.c
    src/accelerometer.c
)

include_directories(include)

target_link_libraries(robot 
    pico_stdlib 
    hardware_spi
    hardware_pwm
)

pico_enable_stdio_usb(robot 1)
pico_enable_stdio_uart(robot 0)
pico_add_extra_outputs(robot)