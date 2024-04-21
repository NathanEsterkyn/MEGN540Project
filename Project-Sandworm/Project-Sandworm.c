#include "Message_Handling.h"  // for translating USB messages to microcontroller tasks
#include "SerialIO.h"          // for USB communication
#include "Task_Management.h"   // for clean task management with functors
#include "Timing.h"            // for Time understanding
#include "Encoder.h"
#include "Battery_Monitor.h"
#include "MotorPWM.h"
#include "Controller.h"
#include "Stepper.h"
#include "Sandworm.h"

#include "Lab1_Tasks.h"
#include "Lab2_Tasks.h"
#include "Lab3_Tasks.h"
#include "Lab4_Tasks.h"
#include "Lab5_Tasks.h"
#include "Project_Tasks.h"
#include "Filter.h"

void Initialize_Modules(float unused)
{
    USB_Flush_Input_Buffer(); // reset USB input buffers

    // Initialize all modules except USB (it can only be called once without messing things up)
    Initialize_Timing();

    // Set up task handling
    Initialize_Task( &task_restart, Initialize_Modules );

    // Set up message handling to get processed at some desired rate.
    Initialize_Task( &task_message_handling, Task_Message_Handling );

    // Set up timing functionality
    Initialize_Task( &task_time_loop, Send_Loop_Time );
    Initialize_Task( &task_send_time, Send_Time_Now );

    // Set up Sandworm functionality
    Initialize_Sandworm( &Sandworm_Robot, 0, 0, 0.010 );
    Initialize_Task( &task_stop_step, Stop_Step );
    Initialize_Task( &task_step_linear, Step_Linear );
    Initialize_Task( &task_step_rotary, Step_Rotary );
    //Initialize_Task( &task_home, Home );
    //Initialize_Task( &task_erase, Erase );
    //Initialize_Task( &task_disable_motors, Disable_Motors );

    // Set up task message handling watchdog
    Initialize_Task( &task_message_handling_watchdog, Task_Message_Handling_Watchdog );

    // Activate message handling to run continuously
    Task_Activate( &task_message_handling, 0 );

    // Activate battery check task to run continuously
    Task_Activate( &task_check_battery_voltage, 0 );
}

int main()
{
    Initialize_USB(); // must only initialize USB once so its excluded from the modules
    Initialize_Modules(0.0); // runs module initialization function

    for(;;) {  // another way to do while(true)
        Task_USB_Upkeep();

        // Sketching functions run through message handling
        Task_Run_If_Ready( &task_message_handling ); // run message handling if ready
        Task_Run_If_Ready( &task_restart ); // restart if ready

        // Timing Functionality
        Task_Run_If_Ready( &task_send_time );
        Task_Run_If_Ready( &task_time_loop );

        // Sandworm base functions
        Task_Run_If_Ready( &task_stop_step );
        Task_Run_If_Ready( &task_step_linear );
        Task_Run_If_Ready( &task_step_rotary );
        //Task_Run_If_Ready( &task_home );
        //Task_Run_If_Ready( &task_erase );
        //Task_Run_If_Ready( &task_disable_motors );

        if (!task_message_handling_watchdog.is_active){ // if the message handling watchdog isn't active (message timeout functionality)
            Task_Activate( &task_message_handling_watchdog, 250 ); // activate message handling watchdog to run every 0.25 seconds (250 ms)
        }
        Task_Run_If_Ready( &task_message_handling_watchdog ); // run watchdog if ready
    }
    return 0;
}