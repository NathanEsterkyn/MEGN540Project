/**
* filter.h/c defines the functions necessary to implement a z-transform
*/
#ifndef STEPPER_H
#define STEPPER_H
#ifndef RB_LENGTH_F
#define RB_LENGTH_F 8  // must be a power of 2 (max of 256) and larger than the filter order +1.
#endif
#include "Ring_Buffer.h"

typedef struct {
   Ring_Buffer_Float_t position;
} Stepper_t;

/**
 * Function Initialize_Stepper initializes the stepper and creates a stepper object that stores the current position.
 * @param p_step Stepper object to populate
 * @param pos position to update to
 */
void Initialize_Stepper( Stepper_t* p_step, uint16_t pos );

/**
 * Function Stepper_Speed updates the speed of the desired stepper motor object
 * @param p_step pointer to the desired stepper object to manipulate
 * @param Value speed value to update the stepper to
 * @param dt time
 */
void Stepper_Speed( Stepper_t* p_step, uint16_t Value, float dt );