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

/*
 * This file defines lab-spcific tasks for execution
 */

#ifndef LAB4_TASKS_H
#define LAB4_TASKS_H

#include "Task_Management.h"
#include "SerialIO.h"  // USB interface for sending results back to user
#include "Timing.h"    // Timing interface
#include "Filter.h"
#include "Battery_Monitor.h"
#include "MotorPWM.h"

// Put your lab-specific tasks here

Task_t task_time_loop;                  ///<-- Lab 2: Indicates if the system should report time to complete a loop.
Task_t task_send_time;                  ///<-- Lab 2: Indicates if the system should send the current time.
Task_t task_message_handling_watchdog;  ///<-- LAB 2: Watchdog Timer for Flusing USB Input Messaging if incomplete message received.
Task_t task_send_battery_voltage;                  ///<-- Lab 2: Indicates if the system should report time to complete a loop.
Task_t task_send_encoder_value;
Task_t task_send_battery_warning; // create a task for low battery warning
Filter_Data_t voltage_filter; // create a filter for the voltage values

// Put your lab-specific task functionality and data_structures (if necessary) here so it is accessable to both
// message handling and the Lab main loops.
void


#endif  // ifndef LAB4_TASKS_H