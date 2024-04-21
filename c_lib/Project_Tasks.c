#include "Project_Tasks.h"

void Stop_Step(float unused) {
    Task_Cancel( &task_step_linear ); // cancels linear stepping
    Task_Cancel( &task_step_rotary ); // cancels rotary stepping

   // float ret_val = 5.0; // FOR TESTING
    //USB_Send_Msg( "cf", 'S', &ret_val, sizeof( ret_val ) ); // FOR TESTING

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
    // activate standby function on motor driver
}

bool Button_Check(float unused) {
    return true;
    // check if button is pressed
}