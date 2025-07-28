#ifndef __motors_h__
#define __motors_h__

#include <stdint.h>

#define MOTOR_DIRECTION_F 0
#define MOTOR_DIRECTION_B 1
#define MOTOR_DIRECTION_L 2
#define MOTOR_DIRECTION_R 3

void dc_motors_init();
void dc_motors_move(const uint8_t direction, const float power);
void dc_motors_halt();

void dc_motors_move_for_ms(const uint8_t direction, const uint8_t power, const double time);

#endif