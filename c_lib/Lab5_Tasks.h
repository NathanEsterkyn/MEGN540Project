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
    FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

*/

/*
 * This file defines lab-specific tasks for execution
 */

#ifndef LAB5_TASKS_H
#define LAB5_TASKS_H

#include "Task_Management.h"

// Include your lab-specific headers here
// e.g. #include "SerialIO.h"  // necessary for sending arithmetic results back to user
#include "SerialIO.h"  // USB interface for sending results back to user
#include "Timing.h"    // Timing interface
#include "Filter.h"
#include "Battery_Monitor.h"
#include "MotorPWM.h"
#include "Encoder.h"
#include "Message_Handling.h"
#include "Controller.h"

// Put your lab-specific tasks here
// e.g. Task_t task_restart;  ///<-- Lab 1: This flag indicates that the device received a restart command from the host. Default inactive.
Task_t task_send_distance;
Task_t task_send_velocity;
Controller_t Left_Controller;
Controller_t Right_Controller;

// Put your lab-specific task functionality and data_structures (if necessary) here so it is accessible to both
// message handling and the Lab main loops.
// e.g. void Send_Time_Now( float _time_since_last );
static const float Car_Width = 0.086; // central distance between treads in meters
static const float Car_Wheel_Diameter = 0.039; // central distance between treads in meters
static const float MAX_PWM = 1000;
static const float pi = 3.14159;
void Send_Distance(float unused);
void Send_Velocity(float unused);
//float* Skid_Steer(float Lin, float Ang, uint8_t ID); // function to convert linear and angular values into tread-specific values

#endif  // ifndef LAB5_TASKS_H