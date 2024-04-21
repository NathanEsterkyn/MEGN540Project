/**
* Stepper.h/c defines functions to create stepper objects and control stepper motors
*/
#ifndef _MEGN540_STEPPER_H
#define _MEGN540_STEPPER_H

#ifndef RB_LENGTH_F
#define RB_LENGTH_F 16
#endif

#include "Ring_Buffer.h"
#include "stdlib.h"
#include "Timing.h"
#include "Ring_Buffer.h"
#include "SerialIO.h"  // USB interface for sending results back to user

typedef struct {
   int number_of_steps; // number of steps per revolution for motors
   int direction; // current direction of the motor
   int motor_pin_1; // indicate which pins the motors are on - we only need one
   int step_number; // what number of steps the motor is currently on
   float last_step_time;
   float step_delay; // parameter to control speed
} Stepper_t;

/**
 * Function Initialize_Stepper initializes the stepper and creates a stepper object that stores the current position.
 * This function also initializes which pins the stepper will be connected to - future dev
 * @param p_step Stepper object to populate
 * @param number_of_steps number of steps per revolution - stepper specific
 * @param motor_pin_1 identifies which 4 pins the H bridge is on based on AIN2 (MP1)
 */
void Initialize_Stepper( Stepper_t* p_step, int number_of_steps, int motor_pin_1 );

/**
 * Function Stepper_Speed updates the speed of the desired stepper motor object
 * @param p_step pointer to the desired stepper object to manipulate
 * @param Value speed value to update the stepper to (RPM)
 */
float Stepper_Speed( Stepper_t* p_step, float Value );

/**
 * Function Stepper_Step moves the desired stepper motor one step
 * @param p_step pointer to the desired stepper object
 */
void Stepper_Step( Stepper_t* p_step );

void Stepper_Disable( Stepper_t* p_step );

#endif