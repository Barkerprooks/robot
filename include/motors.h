#ifndef __motors_h__
#define __motors_h__

#include <stdint.h>

// attach pins and activate PWM
void motors_init();

// motor functions
void drive_motors_forward(const uint8_t power);
void drive_motors_backward(const uint8_t power);
void drive_motors_cw(const uint8_t power);
void drive_motors_ccw(const uint8_t power);

#endif