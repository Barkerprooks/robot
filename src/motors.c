#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "stdio.h"

#include "motors.h"

#define MOTOR_L_PIN_A 8
#define MOTOR_L_PIN_B 9
#define MOTOR_R_PIN_A 10
#define MOTOR_R_PIN_B 11

void dc_motors_init() {

    uint8_t pins[4] = { MOTOR_L_PIN_A, MOTOR_L_PIN_B, MOTOR_R_PIN_A, MOTOR_R_PIN_B };

    for (uint slice, i = 0; i < 4; i++) {
        // register all the pins as PWM
        gpio_set_function(pins[i], GPIO_FUNC_PWM);
        
        // get the PWM slice linked with the GPIO pin
        slice = pwm_gpio_to_slice_num(pins[i]);

        pwm_set_wrap(slice, 65535);
        
        // initialize the PWM pin
        pwm_set_enabled(slice, true);
    }
}

void dc_motors_move(const uint8_t direction, const float power) {
    uint pwm = (uint) (65535.0 * (power / 100.0));
    
    printf("PWM speed: %u\n", pwm);

    uint slices[] = {
        pwm_gpio_to_slice_num(MOTOR_L_PIN_A), 
        pwm_gpio_to_slice_num(MOTOR_L_PIN_B),
        pwm_gpio_to_slice_num(MOTOR_R_PIN_A), 
        pwm_gpio_to_slice_num(MOTOR_R_PIN_B),
    };

    uint channels[] = {
        pwm_gpio_to_channel(MOTOR_L_PIN_A),
        pwm_gpio_to_channel(MOTOR_L_PIN_B),
        pwm_gpio_to_channel(MOTOR_R_PIN_A),
        pwm_gpio_to_channel(MOTOR_R_PIN_B),
    };

    switch (direction) {
        case MOTOR_DIRECTION_F:
            pwm_set_chan_level(slices[0], channels[0], pwm);
            pwm_set_chan_level(slices[1], channels[1], 0);
            pwm_set_chan_level(slices[2], channels[2], 0);
            pwm_set_chan_level(slices[3], channels[3], pwm);
            break;
        case MOTOR_DIRECTION_B:
            pwm_set_chan_level(slices[0], channels[0], 0);
            pwm_set_chan_level(slices[1], channels[1], pwm);
            pwm_set_chan_level(slices[2], channels[2], pwm);
            pwm_set_chan_level(slices[3], channels[3], 0);
            break;
        case MOTOR_DIRECTION_L:
            pwm_set_chan_level(slices[0], channels[0], 0);
            pwm_set_chan_level(slices[1], channels[1], pwm);
            pwm_set_chan_level(slices[2], channels[2], 0);
            pwm_set_chan_level(slices[3], channels[3], pwm);
            break;
        case MOTOR_DIRECTION_R:
            pwm_set_chan_level(slices[0], channels[0], pwm);
            pwm_set_chan_level(slices[1], channels[1], 0);
            pwm_set_chan_level(slices[2], channels[2], pwm);
            pwm_set_chan_level(slices[3], channels[3], 0);
            break;
    }
}

void dc_motors_halt() {
    uint slices[] = {
        pwm_gpio_to_slice_num(MOTOR_L_PIN_A), 
        pwm_gpio_to_slice_num(MOTOR_L_PIN_B),
        pwm_gpio_to_slice_num(MOTOR_R_PIN_A), 
        pwm_gpio_to_slice_num(MOTOR_R_PIN_B),
    };

    uint channels[] = {
        pwm_gpio_to_channel(MOTOR_L_PIN_A),
        pwm_gpio_to_channel(MOTOR_L_PIN_B),
        pwm_gpio_to_channel(MOTOR_R_PIN_A),
        pwm_gpio_to_channel(MOTOR_R_PIN_B),
    };

    pwm_set_chan_level(slices[0], channels[0], 0);
    pwm_set_chan_level(slices[1], channels[1], 0);
    pwm_set_chan_level(slices[2], channels[2], 0);
    pwm_set_chan_level(slices[3], channels[3], 0);
}

void dc_motors_move_for_ms(const uint8_t direction, const uint8_t power, const double time) {
    double timer = 0.0;

    dc_motors_move(direction, power);
    while (timer < time)
        timer += ((double) time_us_64() / 1000.0) - timer;

    dc_motors_halt();
}