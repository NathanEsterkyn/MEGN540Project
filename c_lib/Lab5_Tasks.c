#include "Lab5_Tasks.h"

void Send_Distance(float unused) {
    // enable PWM
    // get a controller value (update)
    // send the value to the motors
}

void Send_Velocity(float unused) {

}

float* Skid_Steer(float Lin, float Ang, uint8_t ID) {

    struct __attribute__( ( __packed__ ) ) {  // creates a struct for the received floats
        float Left;
        float Right;
    } skid_steer;

    if (ID == 0) { // ID == 0 for position mode, 1 for velocity mode
        skid_steer.Left = Lin - Ang*(Car_Width*0.5);
        skid_steer.Right = Lin + Ang*(Car_Width*0.5);
    }

    return skid_steer;
}