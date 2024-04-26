#include "Battery_Monitor.h"
#include "Controller.h"
#include "Encoder.h"
#include "Filter.h"
#include "Lab1_Tasks.h"
#include "Lab2_Tasks.h"
#include "Message_Handling.h"  // for translating USB messages to microcontroller tasks
#include "Project_Tasks.h"
#include "Sandworm.h"
#include "SerialIO.h"  // for USB communication
#include "Stepper.h"
#include "Task_Management.h"  // for clean task management with functors
#include "Timing.h"           // for Time understanding

void Initialize_Modules( float unused )
{
    USB_Flush_Input_Buffer();  // reset USB input buffers

    // Initialize all modules except USB (it can only be called once without messing things up)
    Initialize_Timing();

    // Set up task handling
    Initialize_Task( &task_restart, Initialize_Modules );

    // Set up message handling to get processed at some desired rate.
    Initialize_Task( &task_message_handling, Task_Message_Handling );

    // Set up timing functionality
    Initialize_Task( &task_time_loop, Send_Loop_Time );
    Initialize_Task( &task_send_time, Send_Time_Now );

    // Set up debug functionality
    Initialize_Task( &task_send_switch_status, &Send_Switch_Status );

    // Set up encoders
    Initialize_Encoders();
    Initialize_Task( &task_send_encoder_counts, &Send_Encoder_Counts );

    // Set up Sandworm functionality
    Initialize_Sandworm( &Sandworm_Robot, 0, 0, 22860.0 );
    Initialize_Task( &task_stop_step, Stop_Step );
    Initialize_Task( &task_home, Home );
    Initialize_Task( &task_erase, Erase );
    Initialize_Task( &task_enable_motors, Enable_Motors );
    Initialize_Task( &task_disable_motors, Disable_Motors );
    Initialize_Task( &task_button_check, Button_Check );

    // Set up task message handling watchdog
    Initialize_Task( &task_message_handling_watchdog, Task_Message_Handling_Watchdog );

    // Activate message handling to run continuously
    Task_Activate( &task_message_handling, 0 );

    // Activate button check task to run continuously
    Task_Activate( &task_button_check, 0 );
}

int main()
{
    Initialize_USB();           // must only initialize USB once so its excluded from the modules
    Initialize_Modules( 0.0 );  // runs module initialization function

    for( ;; ) {  // another way to do while(true)
        Task_USB_Upkeep();

        // Sketching functions run through message handling
        Task_Run_If_Ready( &task_message_handling );  // run message handling if ready
        Task_Run_If_Ready( &task_restart );           // restart if ready

        // Timing Functionality
        Task_Run_If_Ready( &task_send_time );
        Task_Run_If_Ready( &task_time_loop );

        // Sandworm base functions
        Task_Run_If_Ready( &task_stop_step );
        Task_Run_If_Ready( &task_home );
        Task_Run_If_Ready( &task_erase );
        Task_Run_If_Ready( &task_enable_motors );
        Task_Run_If_Ready( &task_disable_motors );

        if( !task_message_handling_watchdog.is_active ) {           // if the message handling watchdog isn't active (message timeout functionality)
            Task_Activate( &task_message_handling_watchdog, 250 );  // activate message handling watchdog to run every 0.25 seconds (250 ms)
        }
        Task_Run_If_Ready( &task_message_handling_watchdog );  // run watchdog if ready
    }
    return 0;
}