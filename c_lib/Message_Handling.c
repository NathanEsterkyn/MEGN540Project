/*
MEGN540 Mechatronics Lab
Copyright (C) Andrew Petruska, 2021.
apetruska [at] mines [dot] edu
 www.mechanical.mines.edu
*/

/*
Copyright (c) 2021 Andrew Petruska at Colorado School of Mines

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*/

#include "Message_Handling.h"

/**
* Function _Message_Length returns the number of bytes associated with a command string per the
* class documentation; This is only accessable from this file.
* @param cmd
* @return Size of expected string. Returns 0 if unrecognized.
*/
static uint8_t _Message_Length( char cmd );

/**
* Function Task_Message_Handling processes USB messages as necessary and sets
* status flags to control the flow of the program.
*/

void Task_Message_Handling( float _time_since_last )
{
    if( !USB_Msg_Length() ) { // if there is nothing to process...
        return;
    }
    bool command_processed = false; // make sure task_message_handling_watchdog doesnt reset before a command is processed
    char command = USB_Msg_Peek(); // use Peek to get the operator without removing it so the process keeps going

    switch( command ) { // process operator using a switch statement
        case '*':
            if( USB_Msg_Length() >= _Message_Length( '*' ) ) { // then process your multiplication...

                USB_Msg_Get();  // removes the first character from the received buffer,
                                // we know it is '*' so it isn't saved as a variable
                struct __attribute__( ( __packed__ ) ) { // makes a struct called data with two floats
                    float v1;
                    float v2;
                } data;

                USB_Msg_Read_Into( &data, sizeof( data ) ); // copies bytes from usb receive buffer to struct

                Multiply_And_Send( data.v1, data.v2 ); // does the multiplication, sends the usb message

                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case '/':
            if( USB_Msg_Length() >= _Message_Length( '/' ) ) { // then process your divide...

                USB_Msg_Get();  // removes the first character from the received buffer,
                                // we know it is '/' so it isn't saved as a variable

                struct __attribute__( ( __packed__ ) ) { // makes a struct called data with two floats
                    float v1;
                    float v2;
                } data;

                USB_Msg_Read_Into( &data, sizeof( data ) ); // copies bytes from usb receive buffer to struct

                Divide_And_Send( data.v1, data.v2 ); // does the division, sends the usb message

                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case '+':
            if( USB_Msg_Length() >= _Message_Length( '+' ) ) { // then process your addition...

                USB_Msg_Get();  // removes the first character from the received buffer,
                                // we know it is '+' so it isn't saved as a variable

                struct __attribute__( ( __packed__ ) ) { // makes a struct called data with two floats
                    float v1;
                    float v2;
                } data;

                USB_Msg_Read_Into( &data, sizeof( data ) ); // copies bytes from usb receive buffer to struct

                Add_And_Send( data.v1, data.v2 ); // does the addition, sends the usb message

                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case '-':
            if( USB_Msg_Length() >= _Message_Length( '-' ) ) { // then process your subtraction...

                USB_Msg_Get();  // removes the first character from the received buffer,
                                // we know it is '-' so it isn't saved as a variable

                struct __attribute__( ( __packed__ ) ) { // makes a struct called data with two floats
                    float v1;
                    float v2;
                } data;

                USB_Msg_Read_Into( &data, sizeof( data ) ); // copies bytes from usb receive buffer to struct

                Subtract_And_Send( data.v1, data.v2 ); // does the subtraction, sends the usb message

                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;

        case '~':
            if( USB_Msg_Length() >= _Message_Length( '~' ) ) { // then process your reset...

                USB_Msg_Get();
                USB_Send_Byte(0);
                Task_Activate(&task_restart, -1);
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
                return;
            }
            break;
        case 't':
            if( USB_Msg_Length() >= _Message_Length( 't' ) ) { // then process your 't'...

                USB_Msg_Get();  // removes the first character from the received buffer,
                                // we know it is 't' so it isn't saved as a variable
                char cmd = USB_Msg_Get(); // get the command character given after the 't' to determine behavior
                switch(cmd){
                    case '0': // if a '0' is sent after the 't' char
                        Task_Activate(&task_send_time,-1); // activates the task to run one iteration of task_send_time
                        command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
                        break;
                    case '1': // if a '1' is sent after the 't' char
                        Task_Activate(&task_time_loop,-1); // activates the task to run one iteration of task_time_loop
                        command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
                        break;
                    default:
                        Task_Activate( &task_message_handling_watchdog, -1 );
                        break;
                }
            }
            break;
        case 'T':
            if(USB_Msg_Length() >= _Message_Length('T')) { // then process your 'T'...

                USB_Msg_Get();  // removes the first character from the received buffer,
                                // we know it is 'T' so it isn't saved as a variable
                char cmd = USB_Msg_Get(); // get the command character given after the 'T' to determine behavior
                float X; // declare a float variable to dictate the frequency of the messages
                USB_Msg_Read_Into( &X, sizeof(X) ); // reads the float value
                switch(cmd){
                    case '0': // if a '0' is sent after the 'T' char
                        if (X > 0){
                            Task_Activate(&task_send_time, X); // activates the task to run one iteration of task_send_time
                        }
                        else {
                            Task_Cancel(&task_send_time); // if X<=0 cancel the task
                        }
                        command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
                        break;
                    case '1': // if a '1' is sent after the 't' char
                        if (X > 0){
                            Task_Activate(&task_time_loop, X); // activates the task to run one iteration of task_time_loop
                        }
                        else {
                            Task_Cancel(&task_time_loop); // if X<=0 cancel the task
                        }
                        command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
                        break;
                    default:
                        Task_Activate( &task_message_handling_watchdog, -1 );
                        break;
                }
            }
            break;
        case 'e':
            if( USB_Msg_Length() == _Message_Length( 'e' ) ) {
                USB_Msg_Get();  // removes the first character from the received buffer
                Task_Activate(&task_send_encoder_value, -1); // sends the current encoder value
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'E':
            if( USB_Msg_Length() >= _Message_Length( 'E' ) ) {

                USB_Msg_Get();  // removes the first character from the received buffer
                struct __attribute__( ( __packed__ ) ) { // creates a struct for the received float
                    float X;
                } data;
                USB_Msg_Read_Into( &data, sizeof( data ) ); // fills the struct with the received float

                if (data.X <= 0) { // if the float received is <= 0, cancel the task
                    Task_Cancel(&task_send_encoder_value);
                    command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
                    break;
                }
                Task_Activate(&task_send_encoder_value, timing); // sends the current encoder value at requested interval [us]
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'b':
            if( USB_Msg_Length() == _Message_Length( 'b' ) ) {
                USB_Msg_Get();  // removes the first character from the received buffer
                Task_Activate(&task_send_battery_voltage, -1); // sends the current voltage
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'B':
            if( USB_Msg_Length() >= _Message_Length( 'B' ) ) {

                USB_Msg_Get();  // removes the first character from the received buffer
                struct __attribute__( ( __packed__ ) ) { // creates a struct for the received float
                    float X;
                } data;
                USB_Msg_Read_Into( &data, sizeof( data ) ); // fills the struct with the received float

                if (data.X <= 0) { // if the float received is <= 0, cancel the task
                    Task_Cancel(&task_send_battery_voltage);
                    command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
                    break;
                }
                Task_Activate(&task_send_battery_voltage, (timing*1000)); // send the battery voltage at requested interval [s]
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'p':
            if( USB_Msg_Length() >= _Message_Length( 'p' ) ) {

                USB_Msg_Get();  // removes the first character from the received buffer
                struct __attribute__( ( __packed__ ) ) { // creates a struct for the received unsigned int values
                    int16_t X;
                    int16_t Y;
                } data;
                USB_Msg_Read_Into( &data, sizeof( data ) ); // fills the struct with the received integers
                // Set the PWM command for the left (first) and right (second) side with the sign indicating
                // direction, if power is in acceptable range.
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'P':
            if( USB_Msg_Length() >= _Message_Length( 'P' ) ) {

                USB_Msg_Get();  // removes the first character from the received buffer
                struct __attribute__( ( __packed__ ) ) { // creates a struct for the received unsigned int/float values
                    int16_t X;
                    int16_t Y;
                    float Z;
                } data;
                USB_Msg_Read_Into( &data, sizeof( data ) ); // fills the struct with the received integers/float
                // Set the PWM command for the left (first) and right (second) side with the sign indicating
                // direction, if power is in acceptable range. The following float provides the duration in ms
                // to have the PWM at the specified value, return to 0 PWM (stopped) once that time duration is
                // reached.
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 's':
            if( USB_Msg_Length() == _Message_Length( 's' ) ) {
                USB_Msg_Get();  // removes the first character from the received buffer
                // Stop PWM and disable motor system
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'S':
            if( USB_Msg_Length() == _Message_Length( 'S' ) ) {
                USB_Msg_Get();  // removes the first character from the received buffer
                // Stop PWM and disable motor system
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'q':
            if( USB_Msg_Length() == _Message_Length( 'q' ) ) {
                USB_Msg_Get();  // removes the first character from the received buffer
                // Send system identification data back to host.
                // (total msg length is 16 bytes to fit one endpoint)
                // time(s) PWM_L PWM_R Encoder_L Encoder_R [float] [int16_t] [int16_t] [int16_t] [int16_t]
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'Q':
            if( USB_Msg_Length() >= _Message_Length( 'Q' ) ) {

                USB_Msg_Get();  // removes the first character from the received buffer
                struct __attribute__( ( __packed__ ) ) { // creates a struct for the received unsigned int/float values
                    float X;
                } data;
                USB_Msg_Read_Into( &data, sizeof( data ) ); // fills the struct with the received integers/float
                // Send the system identification information (above) back to the host every X milliseconds
                // (as specified in the second float). If this float is zero or negative, then the repeat
                // send request is canceled.
                command_processed = true; // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        default: // case for unknown command character (unknown operator)
            USB_Msg_Get(); // clears the unknown operator
            USB_Send_Byte('?'); // sends a '?'
            break;
    }

    //********* MEGN540 -- LAB 2 ************//
    if(command_processed) { // if the task has been completed
        Task_Activate( &task_message_handling_watchdog, -1 );  // reset the watchdog timer
    }
}

/**
* @brief Function Task_Message_Handling_Watchdog clears the USB recieve (deleting all messages) to flush the buffer if a complete message is not recieved
* whithin an appropriate amount of time (say 250ms)
*
* @param _unused_
*/
void Task_Message_Handling_Watchdog( float _unused_ )
{
    USB_Flush_Input_Buffer(); // re-initializes the receive ring buffer
}

/**
* Function _Message_Length returns the number of bytes associated with a
* command string per the class documentation;
* @param cmd
* @return Size of expected string. Returns 0 if unreconized.
*/
static uint8_t _Message_Length( char cmd )
{
    switch( cmd ) {
        case '~': return 1; break;
        case '*': return 9; break;
        case '/': return 9; break;
        case '+': return 9; break;
        case '-': return 9; break;
        case 't': return 2; break;
        case 'T': return 6; break;
        case 'e': return 1; break;
        case 'E': return 5; break;
        case 'b': return 1; break;
        case 'B': return 5; break;
        case 'p': return 5; break;
        case 'P': return 9; break;
        case 's': return 1; break;
        case 'S': return 1; break;
        case 'q': return 1; break;
        case 'Q': return 5; break;
        case 'd': return 9; break;
        case 'D': return 13; break;
        case 'v': return 9; break;
        case 'V': return 13; break;
        default: return 0; break;
    }
}
