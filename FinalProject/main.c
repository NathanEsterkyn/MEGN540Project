#include "Encoder.h"
#include "Limit_Switch.h"
#include "Message_Handling.h"
#include "SerialIO.h"
#include "Stepper.h"
#include "Task_Management.h"
#include "Timing.h"

void Initialize_Modules(float _time_not_used_) {
  // reset USB input buffers
  USB_Flush_Input_Buffer();

  // Initialize all modules except USB (it can only be called once without messing things up)
  Initialize_Timing(0.0);
  Initialize_Limit_Switch();
  Initialize_Power_Button();

  // Setup task handling
  Initialize_Task(&task_restart, &Initialize_Modules);

  // Setup message handling to get processed at some desired rate.
  Initialize_Task(&task_message_handling, &Task_Message_Handling);

  // Timing
  Initialize_Task(&task_time_loop, &Send_Loop_Time);
  Initialize_Task(&task_send_time, &Send_Time_Now);

  // Message Handling
  Initialize_Task(&task_message_handling_watchdog,Task_Message_Handling_Watchdog);
  Task_Activate(&task_message_handling_watchdog, 0.25f);
  Task_Activate(&task_message_handling, 0.f);

  // Limit Switch
  Initialize_Task(&task_send_switch_status, &Send_Switch_Status);
  Initialize_Task(&task_send_button_status, &Send_Button_Status);

  // Sandworm
  Initialize_Sandworm(&Sandworm_Robot, 0, 0, 22860.0);
  Initialize_Task(&task_stop_step, Stop_Step);
  Initialize_Task(&task_home, Home);
  Initialize_Task(&task_erase, Erase);
  Initialize_Task(&task_enable_motors, Enable_Motors);
  Initialize_Task(&task_disable_motors, Disable_Motors);
}

int main(int argc, char **argv) {

  // call initialization stuff
  Initialize_Modules(0.0);
  Initialize_USB();
  Task_Activate(&task_message_handling_watchdog, 0.25f);
  Task_Activate(&task_message_handling, 0.f);

  // DISABLE JTAG
  MCUCR |= (1 << JTD);
  MCUCR |= (1 << JTD);

  // DISABLE SPI
  SPCR = 0x00;
  SPCR = 0x00;

  while (true) {
    // USB
    Task_USB_Upkeep();

    // Timing
    Task_Run_If_Ready(&task_time_loop);
    Task_Run_If_Ready(&task_send_time);

    // Task Management
    Task_Run_If_Ready(&task_message_handling);
    Task_Run_If_Ready(&task_restart);
    Task_Run_If_Ready(&task_message_handling_watchdog);

    // Encoder
    Task_Run_If_Ready(&task_send_encoder_counts);

    // Limit Switch and Power Button
    Task_Run_If_Ready(&task_send_switch_status);
    Task_Run_If_Ready(&task_send_button_status);

    // Sandworm base functions
    Task_Run_If_Ready(&task_stop_step);
    Task_Run_If_Ready(&task_home);
    Task_Run_If_Ready(&task_erase);
    Task_Run_If_Ready(&task_enable_motors);
    Task_Run_If_Ready(&task_disable_motors);
  }
  return 0;
}