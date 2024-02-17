#include "Lab2_Tasks.h"

void Send_Time_Now( float _time_since_last){


    float time = Timing_Get_Time_Sec();

    timing_data.cmd = '0';
    timing_data.time = time;
    USB_Send_Msg("ccf", 't' , &timing_data , sizeof(timing_data));

}

void Send_Loop_Time( float _time_since_last){


    bool loop_completed_flag = false;

    if(!loop_completed_flag){
        loop_completed_flag = true;
        task_time_loop.run_period =0;
    }

    else{

        timing_data.cmd = '1';
        timing_data.time = _time_since_last;
        USB_Send_Msg("ccf", 't' , &timing_data , sizeof(timing_data));

        loop_completed_flag = false;
        task_time_loop.run_period = -1;

    }


}