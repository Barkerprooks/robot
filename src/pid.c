#include "pid.h"

// allows fine-tuning of the gain in the final output
double pid(double p, const double delta, const double kp, const double ki, const double kd) {
    static double last = 0, i = 0; // we need to keep some variables static
    
    double d; // d is calculated in the moment

    // PV is just our p since it's tending towards 0.0 -> P = PV - SP
    i += p;                   // update integral 
    d = ((p - last) / delta); // calculate derivative
    last = p;
    
    return kp * p + ki * i + kd * d; // PID
}