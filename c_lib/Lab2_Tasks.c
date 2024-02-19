#include "Lab2_Tasks.h"

void Send_Time_Now( float _time_since_last){
    char time_cmd; // initialize a character for the time command
    if(task_send_time.run_period<0) { // if the run period was negative (always true for the 't' case)
        time_cmd = 't';
    }
    else { // only possible with the 'T' case
        time_cmd = 'T';
    }
    float time = Timing_Get_Time_Sec(); // get the current time
    timing_data.cmd = '0';
    timing_data.time = time;
    USB_Send_Msg("ccf", time_cmd , &timing_data , sizeof(timing_data)); // print out the command and the time
}

void Send_Loop_Time( float _time_since_last){

    static bool loop_completed_flag = false; // an undeclared flag to tell if the loop has been completed
    static float temp_run_period = 0; // set up a temporary run period

    if(!loop_completed_flag) { // if the loop hasn't been completed
        loop_completed_flag = true; // indicate the loop has been
        temp_run_period = task_time_loop.run_period;
        task_time_loop.run_period = 0; // has the loop run indefinitely
    }
    else {
        char time_cmd; // initialize a character for the time command
        if(temp_run_period<0) { // if the run period was negative (always true for the 't' case)
            time_cmd = 't';
        }
        else { // only possible with the 'T' case
            time_cmd = 'T';
        }
        timing_data.cmd = '1';
        timing_data.time = _time_since_last; // get the time since the loop was last ran
        USB_Send_Msg("ccf", time_cmd , &timing_data , sizeof(timing_data)); // print out the command and the time
        loop_completed_flag = false; // indicate the loop hasn't been completed
        task_time_loop.run_period = temp_run_period; // has the loop run once
    }
}