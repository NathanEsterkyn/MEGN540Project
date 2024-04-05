#include "Controller.h"
/**
 * Function Initialize_Controller sets up the z-transform based controller for the system.
 */
void Initialize_Controller(Controller_t* p_cont, float kp, float* num, float* den, uint8_t order, float update_period) {

    p_cont->kp = kp; // set the P gain
    p_cont->update_period = update_period; // set the update period
    p_cont->controller = Filter_Init( &controller, num, den, order ); // create the filter object
}

/**
 * Function Controller_Set_Target_Velocity sets the target velocity for the
 * controller.
 */
void Controller_Set_Target_Velocity( Controller_t* p_cont, float vel ) {
    p_cont->target_vel = vel;
}

/**
 * Function Controller_Set_Target_Position sets the target postion for the
 * controller, this also sets the target velocity to 0.
 */
void Controller_Set_Target_Position( Controller_t* p_cont, float pos ) {
    p_cont->target_pos = pos;
}

/**
 * Function Controller_Update takes in a new measurement and returns the
 * new control value.
 */
float Controller_Update( Controller_t* p_cont, float measurement, float dt ) {
    float* A = p_cont->controller.numerator; // A coefficient
    float* B = p_cont->controller.denominator; // B coefficient
    float theta_target = 0.0;

    float input_last = Filter_Last_Output(&p_cont->controller);
    float output_this = B(0)*measurement + B(1)*input_last + A(1)*Controller_Last(p_cont);
    float output_last = output_this;
    input_last = measurement;

    if (p_cont->target_vel > 0) {
        theta_target = p_cont->target_vel*dt;
    }
    else {
        theta_target = p_cont->target_pos;
    }
    float U = p_cont->kp*(theta_target - output_this);
    return U;
}

/**
 * Function Controller_Last returns the last control command
 */
float Controller_Last( Controller_t* p_cont) {
    if (rb_length_F(&p_cont->controller.out_list) > 0) { // if there is content in the output buffer
        return rb_get_F(&p_cont->controller.out_list,0); // get latest filtered value from back of output buffer
    }
    return 0;
}

/**
 * Function Controller_SettTo sets the Filter's input and output lists
 * to match the measurement so it starts with zero error.
 */
void Controller_SetTo(Controller_t* p_cont, float measurement ) {
    Filter_SetTo(&p_cont->controller,measurement);
}

/**
 * Function Controller_ShiftBy shifts the Filter's input and output lists
 * by the desired amount. This is helpful when dealing with wrapping.
 */
void Controller_ShiftBy(Controller_t* p_cont, float measurement ) {
    Filter_ShiftBy(&p_cont->controller,measurement);
}