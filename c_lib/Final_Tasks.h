#ifndef FINAL_TASKS_H
#define FINAL_TASKS_H

#include "Sandworm.h"
#include "Stepper.h"
#include "Task_Management.h"

// Task Management
Task_t task_restart;
Task_t task_message_handling;

// Timing
Task_t task_time_loop;
Task_t task_send_time;
Task_t task_message_handling_watchdog;
Task_t get_loop_time;
void Send_Loop_Time(float _time_since_last);
void Send_Time_Now(float _time_since_last);

// Encoder
Task_t task_send_encoder_counts;
void Send_Encoder_Counts(float _time_since_last);
void Send_Encoder_Message(char message_type);

// Limit Switch and power button
Task_t task_send_switch_status;
Task_t task_send_button_status;
void Send_Button_Status(float _time_since_last);
void Send_Button_Message(char message_type);
void Send_Switch_Status(float _time_since_last);
void Send_Switch_Message(char message_type);

// Sandworm
Task_t task_stop_step;
Task_t task_step_linear;
Task_t task_step_rotary;
Task_t task_home;
Task_t task_erase;
Task_t task_enable_motors;
Task_t task_disable_motors;
Task_t task_button_check;
Sandworm_t Sandworm_Robot;

static const float HOME_SPEED_L = -250.0;
static const float HOME_SPEED_R = 20.0;
static const float ERASE_SPEED_L = 36.0;
static const float ERASE_SPEED_R = 30.0;
static const float ERASE_TIME = 190000;

void Stop_Step(float unused);
void Enable_Motors(float unused);
void Home(float unused);
void Erase(float unused);
void Disable_Motors(float unused); // use the standby feature on our motor controllers

#endif