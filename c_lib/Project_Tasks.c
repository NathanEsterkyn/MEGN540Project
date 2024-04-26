#include "Project_Tasks.h"

void Home( float unused )
{
    // Linear Homing
    Sandworm_Robot.Lin_vel = HOME_SPEED_L;
    Stepper_Speed( &Sandworm_Robot.Linear, HOME_SPEED_L );  // set homing speed
    Task_Activate( &task_enable_motors, -1 );               // move linear axis towards home
    if( Sandworm_Robot.limitState == 1 ) {                  // if home button is pressed
        Task_Activate( &task_stop_step, -1 );               // stop the motors
        Sandworm_Robot.Lin_pos = 0.0;                       // set zero
    }
}

void Erase( float unused )
{

    Task_Activate( &task_home, -1 );  // spiral function: home, travel to outside, spiral inwards to 0,0
    Sandworm_Robot.Lin_vel = ERASE_SPEED_L;
    Sandworm_Robot.Rot_vel = ERASE_SPEED_R;
    Stepper_Speed( &Sandworm_Robot.Linear, ERASE_SPEED_L );
    Stepper_Speed( &Sandworm_Robot.Rotary, ERASE_SPEED_R );  // set spiral speeds
    Task_Activate( &task_enable_motors, -1 );
    if( Sandworm_Robot.Lin_pos >= Sandworm_Robot.Radius ) {  // if the position of the linear actuator has reached the radius
        Task_Activate( &task_stop_step, -1 );                // stop the motors
    }
}

void Enable_Motors( float unused )
{
    if( Sandworm_Robot.Lin_vel != 0 ) {
        Stepper_Enable( &Sandworm_Robot.Linear );  // enables motors if their speed is not 0
    }
    if( Sandworm_Robot.Rot_vel != 0 ) {
        Stepper_Enable( &Sandworm_Robot.Rotary );
    }
    float linval = 0.0;
    USB_Send_Msg( "cff", 'E', &linval, sizeof( linval ) );  // print out the command and the time
}

void Disable_Motors( float unused )
{
    float linval = 2.0;
    USB_Send_Msg( "cff", 'D', &linval, sizeof( linval ) );  // print out the command and the time
    Stepper_Disable( &Sandworm_Robot.Linear );              // disables both motors
    Stepper_Disable( &Sandworm_Robot.Rotary );
}

void Stop_Step( float unused )
{
    Task_Cancel( &task_enable_motors );
    Task_Activate( &task_disable_motors, -1 );  // disables motors
    Sandworm_Robot.Lin_vel = 0.0;               // set the velocity to zero
    Sandworm_Robot.Rot_vel = 0.0;
    float linval           = 1.0;
    USB_Send_Msg( "cff", 'S', &linval, sizeof( linval ) );  // print out the command and the time
    Task_Cancel( &task_stop_step );                         // cancels itself
}

void Button_Check( float unused )
{

    if( Sandworm_Robot.buttonState == 1 ) {  // checks if the power button is on
        PORTD |= ( 1 << PORTD1 );            // turns on the power LED
    } else {
        PORTD &= ~( 1 << PORTD1 );
    }
}

void Send_Switch_Status( float _time_since_last )
{
    Send_Switch_Message( 'W' );
}

void Send_Switch_Message( char message_type )
{
    struct __attribute__( ( __packed__ ) ) {
        bool status;
    } data;

    data.status = Sandworm_Robot.limitState;

    USB_Send_Msg( "cb", message_type, &data, sizeof( data ) );
}

// Timing ISRs:

ISR( TIMER1_COMPA_vect )  // performs steps for motor 1 (rotary on PORTB)
{
    Stepper_Step( &Sandworm_Robot.Rotary );  // steps rotary motor once
    if( Sandworm_Robot.Rotary.direction == 1 ) {
        Sandworm_Robot.Rot_pos++;
    }  // increment position depending on direction
    if( Sandworm_Robot.Rotary.direction == 0 ) {
        Sandworm_Robot.Rot_pos--;
    }  // mode is set to clear counter on compare match so no need to reset counter value here
}

ISR( TIMER3_COMPA_vect )  // performs steps for motor 2 (Linear on PORTF)
{
    Stepper_Step( &Sandworm_Robot.Linear );  // steps linear motor once
    if( Sandworm_Robot.Linear.direction == 1 ) {
        Sandworm_Robot.Lin_pos++;
    }  // increment position depending on direction
    if( Sandworm_Robot.Linear.direction == 0 ) {
        Sandworm_Robot.Lin_pos--;
    }
}
ISR( INT0_vect )  // ISR for handling a limit switch press
{
    Sandworm_Robot.limitState = PIND & ( 1 << PIND0 );
    // ADD A METHOD FOR DEBOUNCING!!
}

ISR( INT2_vect )  // ISR for handling a power button press
{
    // Sandworm_Robot.buttonState = !Sandworm_Robot.buttonState;
    //  ADD A METHOD FOR DEBOUNCING!!
}