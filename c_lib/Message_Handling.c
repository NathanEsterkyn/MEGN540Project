#include "Message_Handling.h"

/**
 * Function _Message_Length returns the number of bytes associated with a
 * command string per the class documentation; This is only accessable from this
 * file.
 * @param cmd
 * @return Size of expected string. Returns 0 if unrecognized.
 */
static uint8_t _Message_Length(char cmd);

/**
 * Function Task_Message_Handling processes USB messages as necessary and sets
 * status flags to control the flow of the program.
 */
void Task_Message_Handling(float _time_since_last) {

  // Check to see if their is data in waiting
  if (!USB_Msg_Length())
    return; // nothing to process...

  // Get Your command designator without removal so if their are not enough
  // bytes yet, the command persists
  char command = USB_Msg_Peek();

  bool command_processed = false;

  // process command
  switch (command) {
  case 't':
    if (USB_Msg_Length() >= _Message_Length('t')) {
      // Return the time it requested followed by the time to complete the
      // action specified by the second input char.
      USB_Msg_Get();
      uint8_t action = USB_Msg_Get();
      switch (action) {
      case 0x00:
        Task_Run(&task_send_time);
        break;
      case 0x01:
        Task_Activate(&task_time_loop, -1);
        break;
      }
      command_processed = true;
    }
    break;
  case 'T':
    if (USB_Msg_Length() >= _Message_Length('T')) {
      // Return the time it requested followed by the time to complete the
      // action specified by the second input char and returns the time every X
      // milliseconds. If the time is zero or negative it cancels the request.
      USB_Msg_Get();
      struct __attribute__((__packed__)) {
        char action;
        float time;
      } data;
      USB_Msg_Read_Into(&data, sizeof(data));
      switch (data.action) {
      case 0x00:
        if (data.time > 0) {
          Task_Activate(&task_send_time, data.time);
        } else {
          Task_Cancel(&task_send_time);
        }
        break;
      case 0x01:
        if (data.time > 0) {
          Task_Activate(&task_time_loop, data.time);
        } else {
          Task_Cancel(&task_time_loop);
        }
        break;
      }
      command_processed = true;
    }
    break;
  case 'e':
    if (USB_Msg_Length() >= _Message_Length('e')) {
      USB_Msg_Get();

      Send_Encoder_Message('e');
    }
    break;

  case 'E':
    if (USB_Msg_Length() >= _Message_Length('E')) {
      USB_Msg_Get();

      float repetition_time;

      USB_Msg_Read_Into(&repetition_time, sizeof(repetition_time));

      if (repetition_time > 0) {
        Task_Activate(&task_send_encoder_counts, repetition_time);
      } else {
        Task_Cancel(&task_send_encoder_counts);
      }

      command_processed = true;
    }
    break;

  case 'w':
    if (USB_Msg_Length() >= _Message_Length('w')) {
      USB_Msg_Get();
      Send_Button_Message('b');
      Send_Switch_Message('w');
    }
    break;
  case 'W':
    if (USB_Msg_Length() >= _Message_Length('W')) {
      USB_Msg_Get();

      float repetition_time;

      USB_Msg_Read_Into(&repetition_time, sizeof(repetition_time));

      if (repetition_time > 0) {
        Task_Activate(&task_send_switch_status, repetition_time);
      } else {
        Task_Cancel(&task_send_switch_status);
      }

      command_processed = true;
    }
    break;
  case 's':
    if (USB_Msg_Length() >= _Message_Length('s')) {
      USB_Msg_Get();

      command_processed = true;
    }
    break;
  case 'V':
    if (USB_Msg_Length() >= _Message_Length('V')) {
      USB_Msg_Get(); // removes the first character from the received buffer
      struct __attribute__((
          __packed__)) { // creates a struct for the received floats
        float Speed_L;
        float Speed_R;
        float Time;
      } data;
      // fills the struct with the received floats
      USB_Msg_Read_Into(&data, sizeof(data));
      // set velocities in sandworm object
      Sandworm_Robot.Lin_vel = data.Speed_L;
      Sandworm_Robot.Rot_vel = data.Speed_R;
      // set the speed for each motor based on input
      Stepper_Speed(&Sandworm_Robot.Linear, data.Speed_L);
      Stepper_Speed(&Sandworm_Robot.Rotary, data.Speed_R);

      Enable_Motors(0.0);
      // disable the motors after the specified time ( ms )
      Task_Activate(&task_stop_step, data.Time / 1000);
      command_processed = true; // reset the watchdog timer and activates
                                // task_message_handling_watchdog
    }
    break;
  case 'Y':
    if (USB_Msg_Length() == _Message_Length('Y')) {
      USB_Msg_Get(); // removes the first character from the received buffer
      Task_Activate(&task_erase, 0); // erases the sand table
      command_processed = true;
    }
    break;
  case 'H':
    if (USB_Msg_Length() == _Message_Length('H')) {
      USB_Msg_Get(); // removes the first character from the received buffer
      Task_Activate(&task_home, -1); // homes the sand table to (0,0)
      command_processed = true;
    }
    break;
  case 'X':
    if (USB_Msg_Length() == _Message_Length('X')) {
      USB_Msg_Get(); // removes the first character from the received buffer
      Task_Activate(&task_disable_motors, -1);
      command_processed = true;
    }
    break;
  default:
    // What to do if you dont recognize the command character
    // USB_Send_Str( "?" + command );
    USB_Send_Msg("cc", '?', &command, sizeof(command));
    USB_Flush_Input_Buffer();
    break;
  }

  //********* MEGN540 -- LAB 2 ************//
  if (command_processed) {
    // RESET the WATCHDOG TIMER
    Task_Activate(&task_message_handling_watchdog, 0.25f);
  }
}

/**
 * @brief Function Task_Message_Handling_Watchdog clears the USB recieve
 * (deleting all messages) to flush the buffer if a complete message is not
 * recieved whithin an appropriate amount of time (say 250ms)
 *
 * @param _unused_
 */
void Task_Message_Handling_Watchdog(float _unused_) {
  USB_Flush_Input_Buffer();
}

/**
 * Function _Message_Length returns the number of bytes associated with a
 * command string per the class documentation;
 * @param cmd
 * @return Size of expected string. Returns 0 if unreconized.
 */
static uint8_t _Message_Length(char cmd) {
  switch (cmd) {
  case 't':
    return 2;
    break;
  case 'T':
    return 6;
    break;
  case 'e':
    return 1;
    break;
  case 'E':
    return 5;
    break;
  case 'w':
    return 1;
  case 'W':
    return 5;
  case 's':
    return 1;
  case 'S':
    return 1;
  case 'h':
    return 1;
  case 'd':
    return 1;
  case 'V':
    return 13;
  case 'Y':
    return 1;
  case 'H':
    return 1;
  case 'X':
    return 1;
  default:
    return 0;
    break;
  }
}
