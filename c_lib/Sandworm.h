/**
* Sandworm.h/c defines functions specific to our sandworm robot, controlling both motors
*
*
*/
#ifndef _MEGN540_SANDWORM_H
#define _MEGN540_SANDWORM_H

#include "Filter.h"
#include "Stepper.h"
#include "SerialIO.h"
#include <avr/io.h>         // Board Specific pin definations
#include <avr/interrupt.h>  // for interrupt service routine use
#include <ctype.h>

typedef struct {
    Stepper_t Linear;
    Stepper_t Rotary;
    float Lin_pos;
    float Rot_pos;
    float Lin_vel;
    float Rot_vel;
    float Dt;
    float Radius;
    bool buttonState;
    bool limitState;
    bool ledState;
} Sandworm_t;

/**
* Function Initialize_Sandworm sets up the new robot object
 * @param p_sw pointer to the sandworm object
 * @param Lin_pos current linear position
 * @param Rot_pos current rotational position
 * @param Dt time step
*/
void Initialize_Sandworm( Sandworm_t* p_sw, float Lin_pos, float Rot_pos, float Radius );

/**
 * Function Sandworm_Speed updates the speeds of both motors simultaneously to track a trajectory
 * @param p_sw pointer to the SW object
 */
void Sandworm_Home( Sandworm_t* p_sw );

bool Sandworm_Limit( Sandworm_t* p_sw );

#endif