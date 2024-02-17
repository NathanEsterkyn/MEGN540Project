#include "Lab2_Tasks.h"

void Send_Time_Now( float _time_since_last){
    char time_cmd;
    if(task_send_time.run_period<0){
        time_cmd = 't';
    }
    else{
        time_cmd = 'T';
    }

    float time = Timing_Get_Time_Sec();
    timing_data.cmd = '0';
    timing_data.time = time;
    USB_Send_Msg("ccf", time_cmd , &timing_data , sizeof(timing_data));
}

void Send_Loop_Time( float _time_since_last){

    static bool loop_completed_flag = false; // an undeclared flag to tell if the loop has been completed
    static float temp_run_period = 0;

    if(!loop_completed_flag) { // if the loop hasn't been completed
        loop_completed_flag = true; // indicate the loop has been
        temp_run_period = task_time_loop.run_period;
        task_time_loop.run_period = 0; // has the loop run indefinitely
    }
    else {
        char time_cmd;
        if(temp_run_period<0){
            time_cmd = 't';
        }
        else{
            time_cmd = 'T';
        }
        timing_data.cmd = '1';
        timing_data.time = _time_since_last;
        USB_Send_Msg("ccf", time_cmd , &timing_data , sizeof(timing_data));
        loop_completed_flag = false; // indicate the loop hasn't been completed
        task_time_loop.run_period = temp_run_period; // has the loop run once
    }
}