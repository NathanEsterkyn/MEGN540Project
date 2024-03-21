#include "Lab4_Tasks.h"
#include "Filter.h"

void Send_System_Data(float unused) {
    char command;
    float runPeriod = task_send_system_data.run_period;
    if(runPeriod<0){ // detects whether the command was 'q' or 'Q'
        command = 'q';
    }else{
        command = 'Q';
    }
    struct{ // structure to store encoder values
        float Time;
        int16_t PWM_L;
        int16_t PWM_R;
        int16_t Encoder_L;
        int16_t Encoder_R;
    }data;

    data.Time = Timing_Get_Time_Sec(); // write to structure using functions
    data.PWM_L = MotorPWM_Get_Left();
    data.PWM_R = MotorPWM_Get_Right();
    data.Encoder_L = Encoder_Counts_Left();
    data.Encoder_R = Encoder_Counts_Right();

    USB_Send_Msg("cf4h", command, &data, sizeof(data)); // send USB message
}

bool Battery_Check(float unused) {
    float voltage = Filter_Value(&voltage_filter, Battery_Voltage());
    if (voltage < POWER_OFF){ // check if voltage is so low power must be off
        struct __attribute__((__packed__)) {
            char let[9];
        }msg = {.let = {'P','O','W','E','R',' ','O','F','F'}};
        // Send Warning to Serial that batteries need to be charged
        USB_Send_Msg("c9s",'!', &msg, sizeof(msg));
        return false;
    }
    if (voltage < LOW_POWER){ // check if voltage is low
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