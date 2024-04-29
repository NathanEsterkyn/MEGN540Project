#include "Final_Tasks.h"
#include "Encoder.h"
#include "Limit_Switch.h"
#include "SerialIO.h"

void Send_Time_Now(float _time_since_last) {
  float time_s = Timing_Get_Time_Sec();
  USB_Send_Msg("cf", 't', &time_s, sizeof(time_s));
}

void Send_Loop_Time(float _time_since_last) {
  if (task_time_loop.run_period > 0)
    _time_since_last -= task_time_loop.run_period;
  USB_Send_Msg("cf", 'T', &_time_since_last, sizeof(_time_since_last));
}

void Send_Encoder_Message(char message_type) {
  struct __attribute__((__packed__)) {
    float count;
  } data;
  data.count = (float)Encoder_Counts();
  USB_Send_Msg("cff", message_type, &data, sizeof(data));
}

void Send_Encoder_Counts(float _time_since_last) { Send_Encoder_Message('E'); }

void Send_Switch_Status(float _time_since_last) { Send_Switch_Message('W'); }

void Send_Button_Status(float _time_since_last) { Send_Button_Message('B'); }

void Send_Switch_Message(char message_type) {
  struct __attribute__((__packed__)) {
    bool status;
  } data;
  data.status = Limit_Switch_Status();
  USB_Send_Msg( "cb", message_type, &data, sizeof(data) );
}

void Send_Button_Message(char message_type) {
  struct __attribute__((__packed__)) {
    bool status;
  } data;
  data.status = Power_Button_Status();
  USB_Send_Msg( "cb", message_type, &data, sizeof(data) );
}

void Home(float unused) {
  // Linear Homing
  if ( Sandworm_Robot.limitState == 1 ) {
    return;
  }
  Sandworm_Robot.Lin_vel = HOME_SPEED_L;
  Sandworm_Robot.Rot_vel = 0.0; // prevents rotational from moving while homing
  Stepper_Speed( &Sandworm_Robot.Linear, HOME_SPEED_L ); // set homing speed
  Task_Activate( &task_enable_motors, -1 ); // move linear axis towards home
}

void Erase(float unused) {
  Task_Activate( &task_home, -1 ); // spiral function: home, travel to outside, spiral inwards to 0,0
  if ( Sandworm_Robot.limitState == 0 ) {
    return; // wait until home is reached
  }
  Sandworm_Robot.Lin_vel = ERASE_SPEED_L;
  Sandworm_Robot.Rot_vel = ERASE_SPEED_R; // set vel values
  Stepper_Speed( &Sandworm_Robot.Linear, ERASE_SPEED_L );
  Stepper_Speed( &Sandworm_Robot.Rotary, ERASE_SPEED_R ); // set spiral speeds
  Task_Activate( &task_enable_motors, -1 ); // enable motors
  Task_Activate( &task_stop_step, ERASE_TIME ); // disable after 192 sec
  Task_Cancel( &task_erase );
}

void Enable_Motors(float unused) {
  if ( Sandworm_Robot.Lin_vel < 0 && Limit_Switch_Status() ) { // prevents the linear motor from crashing
  }
  else if ( Sandworm_Robot.buttonState == 0 ) { // only activates if button is on
    if( Sandworm_Robot.Lin_vel != 0 ) {
        Stepper_Enable( &Sandworm_Robot.Linear );  // enables motors if their speed is not 0
    }
    if( Sandworm_Robot.Rot_vel != 0 ) {
        Stepper_Enable( &Sandworm_Robot.Rotary );
    }
  }
}

void Disable_Motors(float unused) {
  cli();
  Stepper_Disable( &Sandworm_Robot.Linear ); // disables both motors
  Stepper_Disable( &Sandworm_Robot.Rotary );
  Task_Cancel( &task_disable_motors );
  sei();
}

void Stop_Step(float unused) {
  cli();
  Task_Cancel( &task_enable_motors );
  Disable_Motors( 0.0 );
  Sandworm_Robot.Lin_vel = 0.0; // set the velocity to zero
  Sandworm_Robot.Rot_vel = 0.0;
  Task_Cancel( &task_stop_step ); // cancels itself
  sei();
}