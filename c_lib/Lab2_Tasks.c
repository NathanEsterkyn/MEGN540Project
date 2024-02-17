#include "Lab2_Tasks.h"

void Send_Time_Now( float _time_since_last){
    float time = Timing_Get_Time_Sec();
    timing_data.cmd = '0';
    timing_data.time = time;
    USB_Send_Msg("ccf", 't' , &timing_data , sizeof(timing_data));
}

void Send_Loop_Time( float _time_since_last){

    static bool loop_completed_flag = false; // an undeclared flag to tell if the loop has been completed

    if(!loop_completed_flag) { // if the loop hasn't been completed
        loop_completed_flag = true; // indicate the loop has been
        task_time_loop.run_period = 0; // has the loop run indefinitely
    }
    else {
        timing_data.cmd = '1';
        timing_data.time = _time_since_last;
        USB_Send_Msg("ccf", 't' , &timing_data , sizeof(timing_data));
        loop_completed_flag = false; // indicate the loop hasn't been completed
        task_time_loop.run_period = -1; // has the loop run once
    }
}