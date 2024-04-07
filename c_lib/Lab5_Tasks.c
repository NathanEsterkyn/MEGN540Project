#include "Lab5_Tasks.h"

void Send_Command(float unused) {

    // Left Motor
    float left_measurement = ( pi*Car_Wheel_Diameter ) * Encoder_Rad_Left()/2*pi; // get a measurement of left motor - radians
    float new_left = Controller_Update( &Left_Controller, left_measurement, Left_Controller.update_period ); // get a new control value from the controller
    new_left = Saturate(new_left,MAX_PWM) + Dead_Band_adj; // saturate the controller and adjust for dead band
    MotorPWM_Set_Left( new_left ); // set the new left motor PWM value

    // Right Motor
    float right_measurement = ( pi*Car_Wheel_Diameter ) * Encoder_Rad_Right()/2*pi;
    float new_right = Controller_Update( &Right_Controller, right_measurement, Right_Controller.update_period );
    new_right = Saturate(new_right,MAX_PWM) + Dead_Band_adj;
    MotorPWM_Set_Right( new_right );

    // Anti-whine shutoff
    /*
    float ccL = new_left;
    float lcL = Controller_Last( &Left_Controller );
    float ccR = new_right;
    float lcR = Controller_Last( &Right_Controller );

    if ((ccL - lcL) && (ccR - lcR) <= 0.00001 ) {
        MotorPWM_Set_Left(0);
        MotorPWM_Set_Right(0);
        MotorPWM_Enable( false );
        Task_Cancel( &task_send_command );
    }
    */
    // FOR TESTING PURPOSES:

    struct __attribute__( ( __packed__ ) ) {
        float distance;
        float PWM;
    } data_L;
    data_L.distance = left_measurement;
    data_L.PWM = new_left;

    struct __attribute__( ( __packed__ ) ) {
        float distance;
        float PWM;
    } data_R;
    data_R.distance = right_measurement;
    data_R.PWM = new_right;

    USB_Send_Msg("c2f", 'L',  &data_L, sizeof( data_L ) );
    USB_Send_Msg("c2f", 'R',  &data_R, sizeof( data_R ) );

    MotorPWM_Enable( true ); // enable motors
}

void Clear_Command(float unused) {
    Initialize_Encoders(); // resets the encoders
    Controller_SetTo( &Left_Controller, 0.0 ); // resets the filter in and out values
    Controller_SetTo( &Right_Controller, 0.0 );
    MotorPWM_Set_Left(0); // resets the PWM values
    MotorPWM_Set_Right(0);
    MotorPWM_Enable( false ); // shuts off the motors
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