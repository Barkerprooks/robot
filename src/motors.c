#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

#include "motors.h"

#define MOTOR_L_PIN_A 8
#define MOTOR_L_PIN_B 9
#define MOTOR_R_PIN_A 10
#define MOTOR_R_PIN_B 11

void dc_motors_init() {
    uint8_t pins[4] = { MOTOR_L_PIN_A, MOTOR_L_PIN_B, MOTOR_R_PIN_A, MOTOR_R_PIN_B };
    
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

void dc_motors_move(const uint8_t direction, const uint8_t power) {
    switch (direction) {
        case MOTOR_DIRECTION_F:
            pwm_set_gpio_level(MOTOR_L_PIN_A, power);
            pwm_set_gpio_level(MOTOR_L_PIN_B, 0);
            pwm_set_gpio_level(MOTOR_R_PIN_A, 0);
            pwm_set_gpio_level(MOTOR_R_PIN_B, power);
            break;
        case MOTOR_DIRECTION_B:
            pwm_set_gpio_level(MOTOR_L_PIN_A, 0);
            pwm_set_gpio_level(MOTOR_L_PIN_B, power);
            pwm_set_gpio_level(MOTOR_R_PIN_A, power);
            pwm_set_gpio_level(MOTOR_R_PIN_B, 0);
            break;
        case MOTOR_DIRECTION_L:
            pwm_set_gpio_level(MOTOR_L_PIN_A, 0);
            pwm_set_gpio_level(MOTOR_L_PIN_B, power);
            pwm_set_gpio_level(MOTOR_R_PIN_A, 0);
            pwm_set_gpio_level(MOTOR_R_PIN_B, power);
            break;
        case MOTOR_DIRECTION_R:
            pwm_set_gpio_level(MOTOR_L_PIN_A, power);
            pwm_set_gpio_level(MOTOR_L_PIN_B, 0);
            pwm_set_gpio_level(MOTOR_R_PIN_A, power);
            pwm_set_gpio_level(MOTOR_R_PIN_B, 0);
            break;
    }
}

void dc_motors_halt() {
    pwm_set_gpio_level(MOTOR_L_PIN_A, 0);
    pwm_set_gpio_level(MOTOR_L_PIN_B, 0);
    pwm_set_gpio_level(MOTOR_R_PIN_A, 0);
    pwm_set_gpio_level(MOTOR_R_PIN_B, 0);   
}

void dc_motors_move_for_ms(const uint8_t direction, const uint8_t power, const double time) {
    double timer = 0.0;

    dc_motors_move(direction, power);
    while (timer < time)
        timer += ((double) time_us_64() / 1000.0) - timer;

    dc_motors_halt();
}