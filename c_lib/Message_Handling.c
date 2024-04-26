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
    if( !USB_Msg_Length() ) {  // if there is nothing to process...
        return;
    }
    bool command_processed = false;           // make sure task_message_handling_watchdog doesnt reset before a command is processed
    char command           = USB_Msg_Peek();  // use Peek to get the operator without removing it so the process keeps going

    switch( command ) {  // process operator using a switch statement
        case 't':
            if( USB_Msg_Length() >= _Message_Length( 't' ) ) {  // then process your 't'...

                USB_Msg_Get();             // removes the first character from the received buffer,
                                           // we know it is a 't' so it isn't saved as a variable
                char cmd = USB_Msg_Get();  // get the command character given after the 't' to determine behavior
                switch( cmd ) {
                    case '0':                                  // if a '0' is sent after the 't' char
                        Task_Activate( &task_send_time, -1 );  // activates the task to run one iteration of task_send_time
                        command_processed = true;              // reset the watchdog timer and activates task_message_handling_watchdog
                        break;
                    case '1':                                  // if a '1' is sent after the 't' char
                        Task_Activate( &task_time_loop, -1 );  // activates the task to run one iteration of task_time_loop
                        command_processed = true;              // reset the watchdog timer and activates task_message_handling_watchdog
                        break;
                    default: Task_Activate( &task_message_handling_watchdog, -1 ); break;
                }
            }
            break;
        case 'V':
            if( USB_Msg_Length() >= _Message_Length( 'V' ) ) {
                USB_Msg_Get();                            // removes the first character from the received buffer
                struct __attribute__( ( __packed__ ) ) {  // creates a struct for the received floats
                    float Speed_L;
                    float Speed_R;
                    float Time;
                } data;
                USB_Msg_Read_Into( &data, sizeof( data ) );  // fills the struct with the received floats
                Sandworm_Robot.Lin_vel = data.Speed_L;       // set velocities in sandworm object
                Sandworm_Robot.Rot_vel = data.Speed_R;
                Stepper_Speed( &Sandworm_Robot.Linear, data.Speed_L );  // set the speed for each motor based on input
                Stepper_Speed( &Sandworm_Robot.Rotary, data.Speed_R );

                float shitty = 0.2;
                USB_Send_Msg( "cf", 'V', &shitty, sizeof( shitty ) );  // print out the command and the time

                if( true /*Sandworm_Robot.buttonState == 1*/ ) {  // if the button is pressed
                    float shit = 0.1;
                    USB_Send_Msg( "cf", 'V', &shit, sizeof( shit ) );  // print out the command and the time
                    Task_Activate( &task_enable_motors, -1 );          // enable the motors
                    // Task_Activate( &task_stop_step, data.Time ); // disable the motors after the specified time ( ms )
                }
                command_processed = true;  // reset the watchdog timer and activates task_message_handling_watchdog
            }
            break;
        case 'Y':
            if( USB_Msg_Length() == _Message_Length( 'Y' ) ) {
                USB_Msg_Get();                     // removes the first character from the received buffer
                Task_Activate( &task_erase, -1 );  // erases the sand table
                command_processed = true;
            }
            break;
        case 'H':
            if( USB_Msg_Length() == _Message_Length( 'H' ) ) {
                USB_Msg_Get();                    // removes the first character from the received buffer
                Task_Activate( &task_home, -1 );  // homes the sand table to (0,0)
                command_processed = true;
            }
            break;
        case 'X':
            if( USB_Msg_Length() == _Message_Length( 'H' ) ) {
                USB_Msg_Get();  // removes the first character from the received buffer
                Task_Activate( &task_disable_motors, 0.0 );
                command_processed = true;
                case 'w':
                    if( USB_Msg_Length() >= _Message_Length( 'w' ) ) {
                        USB_Msg_Get();

                        Send_Switch_Message( 'w' );
                        command_processed = true;
                    }
                    break;
                case 'W':
                    if( USB_Msg_Length() >= _Message_Length( 'W' ) ) {
                        USB_Msg_Get();

                        float repetition_time;

                        USB_Msg_Read_Into( &repetition_time, sizeof( repetition_time ) );

                        if( repetition_time > 0 ) {
                            Task_Activate( &task_send_switch_status, repetition_time );
                        } else {
                            Task_Cancel( &task_send_switch_status );
                        }

                        command_processed = true;
                    }
                    break;
                default:                   // case for unknown command character (unknown operator)
                    USB_Msg_Get();         // clears the unknown operator
                    USB_Send_Byte( '?' );  // sends a '?'
                    break;
            }

            //********* MEGN540 -- LAB 2 ************//
            if( command_processed ) {                                  // if the task has been completed
                Task_Activate( &task_message_handling_watchdog, -1 );  // reset the watchdog timer
            }
    }
}

/**
 * @brief Function Task_Message_Handling_Watchdog clears the USB recieve (deleting all messages) to flush the buffer if a complete message is not recieved
 * whithin an appropriate amount of time (say 250ms)
 *
 * @param _unused_
 */
void Task_Message_Handling_Watchdog( float unused )
{
    USB_Flush_Input_Buffer();  // re-initializes the receive ring buffer
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
        case 't': return 2; break;
        case 'V': return 9; break;
        case 'Y': return 1; break;
        case 'H': return 1; break;
        case 'X': return 5; break;
        case 'w': return 1; break;
        case 'W': return 5; break;
        default: return 0; break;
    }
}
