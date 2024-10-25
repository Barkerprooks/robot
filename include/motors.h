#ifndef __motors_h__
#define __motors_h__

#include <stdbool.h>
#include <stdint.h>

#define MOTOR_A_PIN_A 8
#define MOTOR_A_PIN_B 9
#define MOTOR_B_PIN_A 10
#define MOTOR_B_PIN_B 11

// attach pins and activate PWM
void initialize_motors();

// motor functions
void drive_motors_forward(const uint8_t power);
void drive_motors_backward(const uint8_t power);
void drive_motors_left(const uint8_t power);
void drive_motors_right(const uint8_t power);

#endif