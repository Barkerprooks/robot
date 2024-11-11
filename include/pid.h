#ifndef __pid_h__
#define __pid_h__

float pid0(const float p, const float delta);
float pid0_with_gain(const float p, const float delta, const float kp, const float ki, const float kd);

#endif