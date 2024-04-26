#include "Project_Tasks.h"

void Home(float unused) {
    // Linear Homing
    Sandworm_Robot.Lin_vel = HOME_SPEED_L;
    Stepper_Speed( &Sandworm_Robot.Linear, HOME_SPEED_L ); // set homing speed
    Task_Activate( &task_enable_motors, -1 ); // move linear axis towards home
    if( Sandworm_Robot.limitState == 1 ) { // if home button is pressed
        Task_Activate( &task_stop_step, -1 ); // stop the motors
        Sandworm_Robot.Lin_pos = 0.0; // set zero
    }
}

void Erase(float unused) {

    Task_Activate( &task_home, -1 ); // spiral function: home, travel to outside, spiral inwards to 0,0
    Sandworm_Robot.Lin_vel = ERASE_SPEED_L;
    Sandworm_Robot.Rot_vel = ERASE_SPEED_R;
    Stepper_Speed( &Sandworm_Robot.Linear, ERASE_SPEED_L );
    Stepper_Speed( &Sandworm_Robot.Rotary, ERASE_SPEED_R ); // set spiral speeds
    Task_Activate( &task_enable_motors, -1 );
    if ( Sandworm_Robot.Lin_pos >= Sandworm_Robot.Radius ) { // if the position of the linear actuator has reached the radius
        Task_Activate( &task_stop_step, -1 ); // stop the motors
    }
}

void Enable_Motors(float unused) {
    if ( Sandworm_Robot.Lin_vel != 0 ) {
        Stepper_Enable( &Sandworm_Robot.Linear ); // enables motors if their speed is not 0
    }
    if ( Sandworm_Robot.Rot_vel != 0 ) {
        Stepper_Enable( &Sandworm_Robot.Rotary );
    }
}

void Disable_Motors(float unused) {
    Stepper_Disable( &Sandworm_Robot.Linear ); // disables both motors
    Stepper_Disable( &Sandworm_Robot.Rotary );
}

void Stop_Step(float unused) {
    Task_Cancel( &task_step_linear ); // cancels linear stepping
    Task_Cancel( &task_step_rotary ); // cancels rotary stepping
    Task_Activate( &task_disable_motors, -1 ); // disables motors
    Sandworm_Robot.Lin_vel = 0.0; // set the velocity to zero
    Sandworm_Robot.Rot_vel = 0.0;
    Task_Cancel( &task_stop_step ); // cancels itself
}

void Button_Check(float unused) {

    if ( Sandworm_Robot.buttonState == 1 ) { // checks if the power button is on
        PORTD |= ( 1 << PORTD1 ); // turns on the power LED
        return true;
    }
    else {
        PORTD &= ~( 1 << PORTD1 );
        return false;
    }
}

// Timing ISRs:

ISR( TIMER1_COMPA_vect ) // performs steps for motor 1 (rotary on PORTB)
{
    Stepper_Step( &Sandworm_Robot.Rotary ); // steps rotary motor once
    if ( Sandworm_Robot.Rotary.direction == 1 ) { Sandworm_Robot.Rot_pos ++; } // increment position depending on direction
    if ( Sandworm_Robot.Rotary.direction == 0 ) { Sandworm_Robot.Rot_pos --; }// mode is set to clear counter on compare match so no need to reset counter value here
}

ISR( TIMER3_COMPA_vect ) // performs steps for motor 2 (Linear on PORTF)
{
    Stepper_Step( &Sandworm_Robot.Linear ); // steps linear motor once
    if ( Sandworm_Robot.Linear.direction == 1 ) { Sandworm_Robot.Lin_pos ++; } // increment position depending on direction
    if ( Sandworm_Robot.Linear.direction == 0 ) { Sandworm_Robot.Lin_pos --; }
}
ISR( INT0_vect ) // ISR for handling a limit switch press
{
    Sandworm_Robot.limitState = !Sandworm_Robot.limitState;
    // ADD A METHOD FOR DEBOUNCING!!
}

ISR( INT2_vect ) // ISR for handling a power button press
{
    Sandworm_Robot.buttonState = !Sandworm_Robot.buttonState;
    // ADD A METHOD FOR DEBOUNCING!!
}