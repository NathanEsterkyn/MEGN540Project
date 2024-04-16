/**
* Sandworm
*
*
*/
#ifndef _MEGN540_CONTROLLER_H
#define _MEGN540_CONTROLLER_H

#include "Filter.h"
#include "Stepper.h"

typedef struct { Filter_Data_t controller; float kp; float target_pos; float target_vel; float update_period;} Sandworm_t;

/**
* Function Saturate saturates a value to be within the range.
*/
inline float Saturate( float value, float ABS_MAX )
{
   return (value > ABS_MAX)?ABS_MAX:(value < -ABS_MAX)?-ABS_MAX:value;
}

/**
* Function Initialize_Controller sets up the z-transform based controller for the system.
*/
void Initialize_Controller(Controller_t* p_cont, float kp, float* num, float* den, uint8_t order, float update_period);