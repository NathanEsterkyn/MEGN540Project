/**
* Stepper.h/c defines functions to create stepper objects and control stepper motors
*/
#ifndef STEPPER_H
#define STEPPER_H
#ifndef RB_LENGTH_F
#define RB_LENGTH_F 8
#endif
#include "Ring_Buffer.h"

typedef struct {
   Ring_Buffer_Float_t position;
} Stepper_t;

/**
 * Function Initialize_Stepper initializes the stepper and creates a stepper object that stores the current position.
 * This function also initializes which pins the stepper will be connected to - future dev
 * @param p_step Stepper object to populate
 * @param pos position to update to
 */
void Initialize_Stepper( Stepper_t* p_step, uint16_t pos );

/**
 * Function Stepper_Speed updates the speed of the desired stepper motor object
 * @param p_step pointer to the desired stepper object to manipulate
 * @param Value speed value to update the stepper to
 * @param dt time step
 */
void Stepper_Speed( Stepper_t* p_step, uint16_t Value, float dt );