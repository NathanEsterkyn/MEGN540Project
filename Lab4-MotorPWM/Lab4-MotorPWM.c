/*
         MEGN540 Mechatronics Lab
    Copyright (C) Andrew Petruska, 2021.
       apetruska [at] mines [dot] edu
          www.mechanical.mines.edu
*/

/*
    Copyright (c) 2021 Andrew Petruska at Colorado School of Mines

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#include "Message_Handling.h"  // for translating USB messages to microcontroller tasks
#include "SerialIO.h"          // for USB communication
#include "Task_Management.h"   // for clean task management with functors
#include "Timing.h"            // for Time understanding
#include "Encoder.h"
#include "Battery_Monitor.h"
#include "MotorPWM.h"

// Include Lab Specific Functionality
#include "Lab1_Tasks.h"
#include "Lab2_Tasks.h"
#include "Lab3_Tasks.h"
#include "Lab4_Tasks.h"
#include "Filter.h"

void Initialize_Modules(float unused)
{
    USB_Flush_Input_Buffer(); // reset USB input buffers

    // Initialize all modules except USB (it can only be called once without messing things up)
    Initialize_Timing();
    Initialize_Encoders();
    Initialize_Battery_Monitor();
    Initialize_MotorPWM(1000);

    // Set up voltage filter
    float den[] = {1, -1.8669, 0.8752};
    float num[] = {0.0021, 0.0042, 0.0021};
    Filter_Init( &voltage_filter, num, den, 2 );
    Filter_ShiftBy(&voltage_filter, Battery_Voltage());

    // Set up task handling
    Initialize_Task( &task_restart, Initialize_Modules /*function pointer to call*/ );

    // Set up message handling to get processed at some desired rate.
    Initialize_Task( &task_message_handling, Task_Message_Handling );

    // Set up timing functionality
    Initialize_Task(&task_time_loop, Send_Loop_Time);
    Initialize_Task(&task_send_time, Send_Time_Now);

    // Set up encoder and battery voltage functionality
    Initialize_Task(&task_send_encoder_value, Send_Encoder_Value);
    Initialize_Task(&task_check_battery_voltage, Check_Battery_Voltage);
    Initialize_Task(&task_send_battery_voltage, Send_Battery_Voltage);
    Initialize_Task(&task_send_battery_warning, Send_Battery_Warning);

    // Set up PWM functionality
    Initialize_Task(&task_set_pwm_value, Set_PWM_Value);

    // Set up task message handling watchdog
    Initialize_Task( &task_message_handling_watchdog, Task_Message_Handling_Watchdog );

    // Activate message handling to run continuously
    Task_Activate(&task_message_handling,0);

    // Activate battery check task to run continuously and warning task to run every second
    Task_Activate(&task_check_battery_voltage,0);
    Task_Activate(&task_send_battery_warning,1000);
}

int main(){

    Initialize_USB(); // must only initialize USB once so its excluded from the modules
    Initialize_Modules(0.0); // runs module initialization function

    for(;;) {  // another way to do while(true)
        Task_USB_Upkeep();

        Task_Run_If_Ready(&task_message_handling); // run message handling if ready
        Task_Run_If_Ready(&task_restart); // restart if ready

        // Timing Functionality
        Task_Run_If_Ready(&task_send_time);
        Task_Run_If_Ready(&task_time_loop);

        // Encoder and Voltage Functionality
        Task_Run_If_Ready(&task_send_encoder_value);
        Task_Run_If_Ready(&task_send_battery_voltage);
        Task_Run_If_Ready(&task_send_battery_warning);

        // PWM Functionality
        Task_Run_If_Ready(&task_set_pwm_value);

        if (!task_message_handling_watchdog.is_active){ // if the message handling watchdog isn't active (message timeout functionality)
            Task_Activate(&task_message_handling_watchdog,250); // activate message handling watchdog to run every 0.25 seconds (250 ms)
        }
        Task_Run_If_Ready(&task_message_handling_watchdog); // run watchdog if ready
    }
    return 0;
}