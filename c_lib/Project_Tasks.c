#include "Project_Tasks.h"

void Stop_Step(float unused) {
    Task_Cancel( &task_step_linear ); // cancels linear stepping
    Task_Cancel( &task_step_rotary ); // cancels rotary stepping
    Task_Activate( &task_disable_motors, -1 ); // disables motors
    Task_Cancel( &task_stop_step ); // cancels itself
}

void Step_Linear(float unused) {
    Stepper_Step( &Sandworm_Robot.Linear ); // steps linear motor once
    Sandworm_Robot.Lin_pos += 1.0;
}

void Step_Rotary(float unused) {
    Stepper_Step( &Sandworm_Robot.Rotary ); // steps rotary motor once
    Sandworm_Robot.Rot_pos += 1.0;
}

void Home(float unused) {
    // Linear Homing
    float homeSpeed = Stepper_Speed( &Sandworm_Robot.Linear, HOME_SPEED ); // set homing speed
    Task_Activate( &task_step_linear, homeSpeed ); // move linear axis towards home
        if( Sandworm_Button( &Sandworm_Robot ) == 1 ) { // if home button is pressed
            Task_Activate( &task_stop_step, -1 ); // stop the motors
            Sandworm_Robot.Lin_pos = 0.0; // set zero
        }
}

void Erase(float unused) {
    // spiral function: home, travel to outside, spiral inwards to 0,0
}

void Disable_Motors(float unused) {
    // activate standby function on motor driver or just set the register to low
    Stepper_Disable( &Sandworm_Robot.Linear );
    Stepper_Disable( &Sandworm_Robot.Rotary );
}

bool Button_Check(float unused) {
    return true;
    // check if main power button is pressed
    // turn on button LED
}