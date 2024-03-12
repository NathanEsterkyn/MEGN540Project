#include "Lab4_Tasks.h"
#include "Filter.h"

void Set_PWM_Value(int16_t Left, int16_t Right){
    char command;
    float runPeriod = task_set_pwm_value.run_period; // make a task for setting the PWM value
    if(runPeriod<0){ // detects whether the command was 'b' or 'B'
        command = 'p';
    }else{
        command = 'P';
    }
    MotorPWM_Enable(1); // enable motors
    MotorPWM_Set_Left(Left); // set left motor PWM value
    MotorPWM_Set_Right(Right); // set right motor PWM vale
    USB_Send_Byte(command); // send message with the command
}

bool Battery_Check(float unused){
    float voltage = Filter_Value(&voltage_filter, Battery_Voltage());
    if ( voltage < LOW_BATTERY_THRESHOLD_SW_ON){
        struct __attribute__((__packed__)) {
            char let[7];
            float volt;
        }msg = {.let = {'B','A','T',' ','L','O','W'}, .volt = voltage};
        // Send Warning to Serial that batteries need to be charged
        USB_Send_Msg("c7sf",'!', &msg, sizeof(msg));
        return false;
    }
    else if ((voltage > LOW_BATTERY_THRESHOLD_SW_OFF &&){
        struct __attribute__((__packed__)) {
            char let[9];
        }msg = {.let = {'P','O','W','E','R',' ','O','F','F'}};
        // Send Warning to Serial that batteries need to be charged
        USB_Send_Msg("c9s",'!', &msg, sizeof(msg));
        return false;
    }
    return true;
}
