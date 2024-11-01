#include "pid.h"

#define KP 25.0 // error gain
#define KI 0.0 // integral gain
#define KD 0.8 // derivative gain

// pid algorithm tending towards zero (0.0)
float pid(float p, float delta) {
    static float last = 0, i = 0; // we need to keep some variables static
    
    float d; // d is calculated in the moment

    // PV is just our p since it's tending towards 0 -> P = PV - SP
    i += p;                   // update integral 
    d = ((p - last) / delta); // calculate derivative
    last = p;
    
    return KP * p + KI * i + KD * d; // PID
}