#ifndef ENCODER_H
#define ENCODER_H

#include <avr/interrupt.h> // For Interrupts
#include <avr/io.h>        // For pin input/output access
#include <ctype.h>         // For int32_t type
#include <math.h>          // for M_PI
#include <stdbool.h>       // for bool type

/**
 * Function Initialize_Encoders initializes the encoders, sets up the pin change
 * interrupts, and zeros the initial encoder counts.
 */
void Initialize_Encoders();

/**
 * Function Counts_Left returns the number of counts from the left encoder.
 * @return [int32_t] The count number.
 */
int32_t Encoder_Counts();

/**
 * Function Rad_Left returns the number of radians for the left encoder.
 * @return
 */
float Encoder_Rad();

#endif