#include "Lab3_Tasks.h"
#include "Filter.h"

void Send_Encoder_Value(float unused){
    char command;
    float runPeriod = task_send_encoder_value.run_period;
    if(runPeriod<0){ // detects whether the command was 'e' or 'E'
        command = 'e';
    }else{
        command = 'E';
    }
    struct{ // structure to store encoder values
        float left;
        float right;
    }data;

    data.left = Encoder_Counts_Left(); // write to structure using functions
    data.right = Encoder_Counts_Right();
    USB_Send_Msg("cff", command, &data, sizeof(data)); // send USB message
}

void Send_Battery_Voltage(float unused){
    char command;
    float runPeriod = task_send_battery_voltage.run_period;
    if(runPeriod<0){ // detects whether the command was 'b' or 'B'
        command = 'b';
    }else{
        command = 'B';
    }
    float voltage; // value to store voltage reading
    voltage = Filter_Value(&voltage_filter, Battery_Voltage()); // filter voltage reading value
    USB_Send_Msg("cf", command, &voltage, sizeof(voltage)); // send message with the battery voltage
}

void Check_Battery_Voltage(float unused){ // check the filtered voltage value (should run constantly)
    float voltage = Filter_Value(&voltage_filter, Battery_Voltage());
    if (voltage > LOW_BATTERY_THRESHOLD_SW_OFF && voltage < LOW_BATTERY_THRESHOLD_SW_ON){
        Send_Battery_Warning(0.0);
    }
}

void Send_Battery_Warning(float unused){ // send the battery low message (should run every 1000ms)
    struct __attribute__((__packed__)) {
        char let[7];
        float volt;
    }msg = {.let = {'B','A','T',' ','L','O','W'}, .volt = voltage};
    // Send Warning to Serial that batteries need to be charged
    USB_Send_Msg("c7sf",'!', &msg, sizeof(msg));
}
