#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include "Task_Management.h"
#include "stdbool.h"

/**
 * @brief Function Initializes the Limit Switch object and sets up pin change interrupts on INT0 - PD0
 */
void Initialize_Limit_Switch();

/**
 * @brief Function returns the current Limit Switch status as a boolean
 */
bool Limit_Switch_Status();

/**
 * @brief Function Initializes the Power Button object and sets up pin change interrupts on INT2 - PD2
 */
void Initialize_Power_Button();

/**
 * @brief Function returns the current Power Button status as a boolean
 */
bool Power_Button_Status();

#endif