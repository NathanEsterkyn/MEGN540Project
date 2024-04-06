#include "Lab5_Tasks.h"

void Send_Distance(float unused) {

    // Left Motor
    float left_measurement = Encoder_Rad_Left(); // get a measurement of left motor - radians
    float left_error = ( Left_Controller.target_pos - ( pi*Car_Wheel_Diameter ) * ( left_measurement/2*pi ) ); // calculate the position error
    float new_left = Controller_Update( Left_Controller, left_error, Left_Controller.update_period ); // get a new control value from the controller
    new_left = Saturate(new_left,MAX_PWM); // saturate the controller
    MotorPWM_Set_Left( new_left ); // set the new left motor PWM value

    // Right Motor
    float right_measurement = Encoder_Rad_Right();
    float right_error = ( Right_Controller.target_pos - ( pi*Car_Wheel_Diameter ) * ( right_measurement/2*pi ) );
    float new_right = Controller_Update( Right_Controller, right_error, Right_Controller.update_period );
    MotorPWM_Set_Right( new_right );

    MotorPWM_Enable( true ); // enable motors
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