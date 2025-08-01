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

        // Set the max pwm value
        pwm_set_wrap(slice, 65535);
        
        // initialize the PWM pin
        pwm_set_enabled(slice, true);
    }
}

void dc_motor_pin_set_pwm_value(uint pin, double value) {
    pwm_set_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin), value);
}

void dc_motors_move(const uint8_t direction, const float power) {
    uint value = (uint) (65535.0f * (power / 100.0f));
    
    switch (direction) {
        case MOTOR_DIRECTION_F:
            dc_motor_pin_set_pwm_value(MOTOR_L_PIN_A, value);
            dc_motor_pin_set_pwm_value(MOTOR_L_PIN_B, 0.0f);
            dc_motor_pin_set_pwm_value(MOTOR_R_PIN_A, value);
            dc_motor_pin_set_pwm_value(MOTOR_R_PIN_B, 0.0f);
            break;
        case MOTOR_DIRECTION_B:
            dc_motor_pin_set_pwm_value(MOTOR_L_PIN_A, 0.0f);
            dc_motor_pin_set_pwm_value(MOTOR_L_PIN_B, value);
            dc_motor_pin_set_pwm_value(MOTOR_R_PIN_A, 0.0f);
            dc_motor_pin_set_pwm_value(MOTOR_R_PIN_B, value);
            break;
        case MOTOR_DIRECTION_L:
            dc_motor_pin_set_pwm_value(MOTOR_L_PIN_A, 0.0f);
            dc_motor_pin_set_pwm_value(MOTOR_L_PIN_B, value);
            dc_motor_pin_set_pwm_value(MOTOR_R_PIN_A, value);
            dc_motor_pin_set_pwm_value(MOTOR_R_PIN_B, 0.0f);
            break;
        case MOTOR_DIRECTION_R:
            dc_motor_pin_set_pwm_value(MOTOR_L_PIN_A, value);
            dc_motor_pin_set_pwm_value(MOTOR_L_PIN_B, 0.0f);
            dc_motor_pin_set_pwm_value(MOTOR_R_PIN_A, 0.0f);
            dc_motor_pin_set_pwm_value(MOTOR_R_PIN_B, value);
            break;
    }
}

void dc_motors_halt() {
    dc_motor_pin_set_pwm_value(MOTOR_L_PIN_A, 0.0f);
    dc_motor_pin_set_pwm_value(MOTOR_L_PIN_B, 0.0f);
    dc_motor_pin_set_pwm_value(MOTOR_R_PIN_A, 0.0f);
    dc_motor_pin_set_pwm_value(MOTOR_R_PIN_B, 0.0f);
}

void dc_motors_move_for_ms(const uint8_t direction, const float power, const double time) {
    double timer = 0.0;

    dc_motors_move(direction, power);
    while (timer < time)
        timer += ((double) time_us_64() / 1000.0) - timer;

    dc_motors_halt();
}