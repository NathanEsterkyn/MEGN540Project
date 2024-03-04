#include "Task_Management.h"
#include "Timing.h"

/** Function Initialize_Task initializes the task to a default state that is inactive.
 *  Note that a negative run_period indicates the task should only be performed once, while
 *  a run_period of 0 indicates the task should be run every time.
 */
void Initialize_Task( Task_t* task, void ( *task_fcn_ptr )( float ) )
{
    task->is_active              = false;
    task->run_period             = -1;
    task->time_last_ran.microsec = 0;
    task->time_last_ran.millisec = 0;
    task->task_fcn_ptr           = task_fcn_ptr;
}

/** Function Task_Activate changes the internal state to enable the task **/
void Task_Activate( Task_t* task, float run_period )
{
    //****** MEGN540 --  START IN LAB 1, UPDATE IN Lab 2 ******//
    // Here you should change the state of the is_active member and set the time to now (lab 2)
    task->is_active  = true; // sets the status to active - signalling to other functions to run the task
    task->run_period = run_period; // sets the run period to determine if the task runs once or indefinite
    task->time_last_ran = Timing_Get_Time(); // update the time last ran
}

/**
 * @brief Function Task_ReActivate sets the state of is_active to true and resets the time last ran to the current time.
 * This has the same functionality as Task_Activate but it does not allow changing the tasks's run period.
 *
 * @param task is a pointer to the task object of interest
 */
void Task_ReActivate( Task_t* task )
{
    //****** MEGN540 --  START IN LAB 1, UPDATE IN Lab 2 ******//
    // Here you should change the state of the is_active member and set the time to now (lab 2)
    // to identify the task is active
    task->is_active = true; // reactivate task
    task->time_last_ran = Timing_Get_Time(); // update time last ran
}

/** Function Task_Cancel changes the internal state to disable the task **/
void Task_Cancel( Task_t* task )
{
    //****** MEGN540 -- Lab 1 ******//
    // Here you should change the state of the is_active member
    // to identify the task is inactive
    task->is_active = false;
}

/** Function Task_Is_Ready indicates if the task should be run. It checks both
 * the active status and the timing.
 */
bool Task_Is_Ready( Task_t* task )
{
    //****** MEGN540 --  START IN LAB 1, UPDATE IN Lab 2 ******//
    // return true if the task is currently active and if the time since the task last ran is larger than the
    // interval given by the run period, otherwise return false.
    return(task->is_active && ( Timing_Seconds_Since(&task->time_last_ran)>=task->run_period*0.001));
}

/**
 * @brief Function Task_Run runs the task-function (if not NULL) and updates the time last ran to the current time. If the run_period is -1, this also
 * deactivates the task.
 *
 * @param task pointer to the task object of interest
 */
void Task_Run( Task_t* task )
{
    // If the function pointer is not NULL (0), run task.
    // To call a void functor (function pointer):  functor_variable();
    // Update time_last_ran and is_active as appropriate.
    // Note that a negative run_period indicates the task should only be performed once, while
    // a run_period of 0 indicates the task should be run every time if it is active.
    task->task_fcn_ptr(Timing_Seconds_Since(&task->time_last_ran));
    task->time_last_ran = Timing_Get_Time();
    if(task->run_period < 0){ // if the task run period is negative, cancel the task after it has been run once
        Task_Cancel(task);
    }
}

/** Function Task_Run_If_Ready Function Task_Run_If_Ready checks to see if the given task is ready for execution, executes the task,
 *  and resets the time_last_ran appropriately. If the task function pointer is NULL then it just
 *  returns if the task is ready and resets the time_last_ran.
 */
bool Task_Run_If_Ready( Task_t* task )
{
    //****** MEGN540 --  START IN LAB 1, UPDATE IN Lab 2   ******//
    if( Task_Is_Ready(task) ) { // run the task if its ready
        Task_Run(task);
        return true;
    }
    return false;
}
