cmake_minimum_required(VERSION 3.13)

include(pico-sdk/pico_sdk_init.cmake)

project(robot)

pico_sdk_init()

add_executable(robot 
    src/main.c
    src/pid.c
    src/motors.c
    src/machine.c
    src/service.c
    src/sixaxis.c
)

include_directories(include)

target_link_libraries(robot 
    pico_stdlib 
    hardware_i2c
    hardware_pwm
    pico_cyw43_arch_lwip_threadsafe_background
)

pico_enable_stdio_usb(robot 1)
pico_enable_stdio_uart(robot 0)
pico_add_extra_outputs(robot)
