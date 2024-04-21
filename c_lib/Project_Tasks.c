#include "Project_Tasks.h"

void Stop_Step(float unused) {
    Task_Cancel( &task_step_linear ); // cancels linear stepping
    Task_Cancel( &task_step_rotary ); // cancels rotary stepping
    Task_Activate( &task_disable_motors, -1 ); // disables motors
    Task_Cancel( &task_stop_step ); // cancels itself
}

void Step_Linear(float unused) {
    Stepper_Step( &Sandworm_Robot.Linear ); // steps linear motor once
}

void Step_Rotary(float unused) {
    Stepper_Step( &Sandworm_Robot.Rotary ); // steps rotary motor once
}

void Home(float unused) {
    // homing function
}

void Erase(float unused) {
    // spiral function
}

void Disable_Motors(float unused) {
    // activate standby function on motor driver or just set the register to low
    Stepper_Disable( &Sandworm_Robot.Linear );
    Stepper_Disable( &Sandworm_Robot.Rotary );
}

bool Button_Check(float unused) {
    return true;
    // check if button is pressed
}