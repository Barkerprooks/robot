#include "hardware/gpio.h"
#include "hardware/pwm.h"

#include "motors.h"

#define MOTOR_A_PIN_A 8
#define MOTOR_A_PIN_B 9
#define MOTOR_B_PIN_A 10
#define MOTOR_B_PIN_B 11

void motors_init() {
    uint8_t pins[4] = { MOTOR_A_PIN_A, MOTOR_A_PIN_B, MOTOR_B_PIN_A, MOTOR_B_PIN_B };
    
    pwm_config config = pwm_get_default_config();
    
    // configure PWM clkdiv to 4 for some reason
    pwm_config_set_clkdiv(&config, 4.0f);

    for (uint8_t slice, i = 0; i < 4; i++) {
        // register all the pins as PWM
        gpio_set_function(pins[i], GPIO_FUNC_PWM);
        
        // get the PWM slice linked with the GPIO pin
        slice = pwm_gpio_to_slice_num(pins[i]);

        // initialize the PWM pin
        pwm_init(slice, &config, true);
    }
}

void drive_motors_forward(const uint8_t power) {
    // motor A (left) moves CW
    pwm_set_gpio_level(MOTOR_A_PIN_A, power);
    pwm_set_gpio_level(MOTOR_A_PIN_B, 0);
    // motor B (right) moves CCW
    pwm_set_gpio_level(MOTOR_B_PIN_A, 0);
    pwm_set_gpio_level(MOTOR_B_PIN_B, power);
}

void drive_motors_backward(const uint8_t power) {
    // motor A (left) moves CCW
    pwm_set_gpio_level(MOTOR_A_PIN_A, 0);
    pwm_set_gpio_level(MOTOR_A_PIN_B, power);
    // motor B (right) moves CW
    pwm_set_gpio_level(MOTOR_B_PIN_A, power);
    pwm_set_gpio_level(MOTOR_B_PIN_B, 0);
}

void drive_motors_cw(const uint8_t power) {
    // motor A (left) moves CW
    pwm_set_gpio_level(MOTOR_A_PIN_A, 0);
    pwm_set_gpio_level(MOTOR_A_PIN_B, power);
    // motor B (right) moves CW
    pwm_set_gpio_level(MOTOR_B_PIN_A, 0);
    pwm_set_gpio_level(MOTOR_B_PIN_B, power);
}

void drive_motors_ccw(const uint8_t power) {
    // motor A (left) moves CCW
    pwm_set_gpio_level(MOTOR_A_PIN_A, power);
    pwm_set_gpio_level(MOTOR_A_PIN_B, 0);
    // motor B (right) moves CCW
    pwm_set_gpio_level(MOTOR_B_PIN_A, power);
    pwm_set_gpio_level(MOTOR_B_PIN_B, 0);
}