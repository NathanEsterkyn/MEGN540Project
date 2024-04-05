#include "Skid_Steer_Controller.h"
/**
* @brief Initialize_Skid_Steer initializes the skid steer object that is statically created in the c file
*
* @param z_transform_numerator the controller's z-transform numerator
* @param z_transform_denominator the controller's z-transform denominator
* @param z_transform_order the controller's order
* @param discretization_period the period used to discretize the z-transform coefficients
* @param error_to_control_gain the conversion from error (counts? or rad?) to control (pwm?) for the controller
* @param max_abs_control the absolute valued maximum control for saturation in control units (pwm?)
* @param wheel_base_width the axel-width between treads to help convert from left-right imbalance to car rotation
* @param wheel_diameter  the diameter of the wheels to convert from wheel rotation to translation
*
* // Optional function pointers to assist with making this code more generic
* @param measurement_left_fcn_ptr a function pointer to the left-side measurement
* @param measurement_right_fcn_ptr a function pointer to the right-side measurement
* @param control_left_fcn_ptr a function pointer to the left side's control application
* @param control_right_fcn_ptr a function pointer to the right side's control application
*/
void Initialize_Skid_Steer( Skid_Steer_Controller_t* p_skid_steer, float* z_transform_numerator, float* z_transform_denominator, uint8_t z_transform_order,
                           float discretization_period, float error_to_control_gain, float max_abs_control, float wheel_base_width, float wheel_diameter,
                           float ( *measurement_left_fcn_ptr )( void ), float ( *measurement_right_fcn_ptr )( void ), void ( *control_left_fcn_ptr )( float ),
                           void ( *control_right_fcn_ptr )( float ) );

/**
* @brief Skid_Steer_Command_Displacement sets a new target displacement for the robot to execute. This is a relative displacement to the current position, not an
* absolute target.
*
* @param linear The arc-length to travel (m)
* @param angular The angle to rotate (rad)
*/
void Skid_Steer_Command_Displacement( Skid_Steer_Controller_t* p_skid_steer, float linear, float angular );

/**
* @brief Skid_Steer_Command_Velocity sets a target velocity for the skid-steer system to execute
*
* @param linear The tangential velocity to move at (m/s)
* @param angular The angular rate to spin at (rad/s)
*/
void Skid_Steer_Command_Velocity( Skid_Steer_Controller_t* p_skid_steer, float linear, float angular );

/**
* @brief Skid_Steer_Control_Update executes a control update by comparing current measurments desired values and implements new control setpoints.
*
*/
void Skid_Steer_Control_Update( Skid_Steer_Controller_t* p_skid_steer, float ellapsed_time );