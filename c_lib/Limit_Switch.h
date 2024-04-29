#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include "Task_Management.h"
#include "stdbool.h"

void Initialize_Limit_Switch();

bool Limit_Switch_Status();

void Initialize_Power_Button();

bool Power_Button_Status();

#endif