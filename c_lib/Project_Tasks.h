#ifndef PROJECT_TASKS_H
#define PROJECT_TASKS_H

#include "Task_Management.h"

// Include your lab-specific headers here
// e.g. #include "SerialIO.h"  // necessary for sending arithmetic results back to user
#include "SerialIO.h"  // USB interface for sending results back to user
#include "Timing.h"    // Timing interface
#include "Filter.h"
#include "Battery_Monitor.h"
#include "MotorPWM.h"
#include "Encoder.h"
#include "Message_Handling.h"
#include "Controller.h"
#include "Sandworm.h"
#include "Stepper.h"

Task_t task_stop_step;
Task_t task_step_linear;
Task_t task_step_rotary;
Task_t task_home;
Task_t task_erase;
Task_t task_disable_motors;
Sandworm_t Sandworm_Robot;

static const float HOME_SPEED_L = 100.0;
static const float HOME_SPEED_R = 20.0;
static const float ERASE_SPEED_L = 300.0;
static const float ERASE_SPEED_R = 30.0;

void Stop_Step(float unused);
void Step_Linear(float unused);
void Step_Rotary(float unused);
void Home(float unused);
void Erase(float unused);
void Disable_Motors(float unused); // use the standby feature on our motor controllers
bool Button_Check(float unused); // function to determine if the button is pressed or not

#endif