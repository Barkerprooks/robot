cmake_minimum_required(VERSION 3.13)

include(/home/parker/.pico-sdk/pico_sdk_init.cmake)

project(robot)

pico_sdk_init()

add_executable(robot 
    src/main.c
    src/motors.c
    src/sixaxis.c
)

include_directories(include)

target_link_libraries(robot 
    pico_stdlib 
    hardware_i2c
    hardware_pwm
)

pico_enable_stdio_usb(robot 1)
pico_enable_stdio_uart(robot 0)
pico_add_extra_outputs(robot)
