#include "Lab4_Tasks.h"
#include "Filter.h"

void Set_PWM_Value(float unused){
    char command;
    float runPeriod = task_set_pwm_value.run_period; // make a task for setting the PWM value
    if(runPeriod<0){ // detects whether the command was 'p' or 'P'
        command = 'p';
    }else{
        command = 'P';
    }
    struct{ // structure to store PWM values
        int16_t left;
        int16_t right;
    }data;
    data.left = MotorPWM_Get_Left(); // write to structure using functions
    data.right = MotorPWM_Get_Right();
    USB_Send_Msg("cff", command, &data, sizeof(data)); // send USB message
}

void Stop_PWM(float unused){
    char command;
    float runPeriod = task_stop_pwm.run_period; // make a task for setting the PWM value
    if(runPeriod<0){ // detects whether the command was 's' or 'S'
        command = 's';
    }else{
        command = 'S';
    }
    USB_Send_Byte(command); // send USB message
}

bool Battery_Check(float unused){
    float voltage = Filter_Value(&voltage_filter, Battery_Voltage());
    if (voltage < 1.0){
        struct __attribute__((__packed__)) {
            char let[9];
        }msg = {.let = {'P','O','W','E','R',' ','O','F','F'}};
        // Send Warning to Serial that batteries need to be charged
        USB_Send_Msg("c9s",'!', &msg, sizeof(msg));
        return false;
    }
    if (voltage < 3.6){
        struct __attribute__((__packed__)) {
            char let[7];
            float volt;
        }msg = {.let = {'B','A','T',' ','L','O','W'}, .volt = voltage};
        // Send Warning to Serial that batteries need to be charged
        USB_Send_Msg("c7sf",'!', &msg, sizeof(msg));
        return false;
    }
    else {
        return true;
    }
}
