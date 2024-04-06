#include "Lab5_Tasks.h"

void Send_Distance(float unused) {

    // Left Motor
    float left_measurement = ( pi*Car_Wheel_Diameter ) * Encoder_Rad_Left()/2*pi; // get a measurement of left motor - radians
    float new_left = Controller_Update( &Left_Controller, left_measurement, Left_Controller.update_period ); // get a new control value from the controller
    new_left = Saturate(new_left,MAX_PWM); // saturate the controller
    MotorPWM_Set_Left( new_left ); // set the new left motor PWM value

    // Right Motor
    float right_measurement = ( pi*Car_Wheel_Diameter ) * Encoder_Rad_Right()/2*pi;
    float new_right = Controller_Update( &Right_Controller, right_measurement, Right_Controller.update_period );
    new_right = Saturate(new_right,MAX_PWM);
    MotorPWM_Set_Right( new_right );

    struct __attribute__( ( __packed__ ) ) {  // creates a struct for the received floats
        float PWM;
    } data;
    data.PWM = left_measurement;

    MotorPWM_Enable( true ); // enable motors
    USB_Send_Msg("cf", 'L',  &data, sizeof( data ) );
}

void Send_Velocity(float unused) {

}
/*
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
 */