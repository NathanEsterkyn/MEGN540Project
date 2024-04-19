#include "Sandworm.h"

void Initialize_Sandworm( Sandworm_t* p_sw, float Lin_pos, float Rot_pos, float Dt ) {
    p_sw->Lin_pos = Lin_pos; // set linear position
    p_sw->Rot_pos = Rot_pos; // set rotary position
    p_sw->Dt = Dt; // set time step
    Initialize_Stepper( &p_sw->Linear, 0, 200, 8 ); // create the linear stepper object - Motor 1
    Initialize_Stepper( &p_sw->Rotary, 0, 200, 36 ); // create the rotary stepper object - Motor 2
}

void Sandworm_Speed( float Lin_Vel, float Rot_vel ) {

}