#include "Lab3_Tasks.h"
#include "Filter.h"

void Send_Encoder_Value(float unused){

    //static float runPeriod = 0;
    char command;
    float runPeriod = task_send_encoder_value.run_period;
    if(runPeriod<0){
        command = 'e';
    }else{
        command = 'E';
    }
    struct{
        float left;
        float right;
    }data;

    data.left = Encoder_Counts_Left();
    data.right = Encoder_Counts_Right();
    USB_Send_Msg("cff", command, &data, sizeof(data));
}

void Send_Battery_Voltage(float unused){
    char command;
    float runPeriod = task_send_battery_voltage.run_period;
    if(runPeriod<0){
        command = 'b';
    }else{
        command = 'B';
    }
    float voltage;
    voltage = Filter_Value(&voltage_filter, Battery_Voltage());
    USB_Send_Msg("cf", command, &voltage, sizeof(voltage)); // send message with the battery voltage
}

void Send_Battery_Warning(float unused){
    float voltage = Filter_Last_Output(&voltage_filter);
    if (voltage <= LOW_BATTERY_THRESHOLD){
        struct __attribute__((__packed__)) {
            char let[7];
            float volt;
        }msg = {.let = ['B','A','T',' ','L','O','W'], .volt = voltage};
        // Send Warning to Serial that batteries need to be charged
        USB_Send_Msg("c7sf",'!', &msg, sizeof(msg));
    }
}
