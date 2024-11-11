#include "pid.h"

// pid algorithm tending towards zero (0.0)
float pid0(const float p, const float delta) {
    static float last = 0, i = 0; // we need to keep some variables static
    
    float d; // d is calculated in the moment

    // PV is just our p since it's tending towards 0.0 -> P = PV - SP
    i += p;                   // update integral 
    d = ((p - last) / delta); // calculate derivative
    last = p;
    
    return p + i + d; // PID
}

// allows fine-tuning of the gain in the final output
float pid0_with_gain(const float p, const float delta, const float kp, const float ki, const float kd) {
    static float last = 0, i = 0; // we need to keep some variables static
    
    float d; // d is calculated in the moment

    // PV is just our p since it's tending towards 0.0 -> P = PV - SP
    i += p;                   // update integral 
    d = ((p - last) / delta); // calculate derivative
    last = p;
    
    return kp * p + ki * i + kd * d; // PID
}