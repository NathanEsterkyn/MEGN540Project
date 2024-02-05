#include "Lab1_Tasks.h"

// This function takes two values, multiplies them, and sends the result over serial back to the Host
void Multiply_And_Send( float value_left, float value_right )
{
    // Do the thing you need to do. Here we want to multiply
    float ret_val = value_left * value_right;

    // send response right here if appropriate.
    USB_Send_Msg( "cf", '*', &ret_val, sizeof( ret_val ) );
}

void Divide_And_Send( float value_left, float value_right ) {
        // Do the thing you need to do. Here we want to Divide
    float ret_val = value_left / value_right;

    // this might need to check for divide by 0 issues

    // send response right here if appropriate.
    USB_Send_Msg( "cf", '/', &ret_val, sizeof( ret_val ) );
}

void Add_And_Send( float value_left, float value_right ) {
        // Do the thing you need to do. Here we want to ADD
    float ret_val = value_left + value_right;

    // send response right here if appropriate.
    USB_Send_Msg( "cf", '+', &ret_val, sizeof( ret_val ) );
}

void Subtract_And_Send( float value_left, float value_right ) {
        // Do the thing you need to do. Here we want to Subtract
    float ret_val = value_left - value_right;

    // send response right here if appropriate.
    USB_Send_Msg( "cf", '-', &ret_val, sizeof( ret_val ) );
}