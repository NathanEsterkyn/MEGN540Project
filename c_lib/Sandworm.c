#include "Sandworm.h"

void Initialize_Sandworm( Sandworm_t* p_sw, float Lin_pos, float Rot_pos, float Dt ) {
    p_sw->Lin_pos = Lin_pos; // set linear position
    p_sw->Rot_pos = Rot_pos; // set rotary position
    p_sw->Dt = Dt; // set time step
    Initialize_Stepper( &p_sw->Linear, 0, 200, 36 ); // create the linear stepper object - Motor 1
    Initialize_Stepper( &p_sw->Rotary, 0, 200, 8 ); // create the rotary stepper object - Motor 2
}

void Sandworm_Speed( Sandworm_t* p_sw, uint16_t Lin_Vel, int Rot_vel ) { // for testing I am sending it Speed (Lin_Vel) then Steps (Rot_Vel)
    Stepper_Speed( &p_sw->Linear, Lin_Vel ); // sets the speed for the linear stepper - Motor 1
    Stepper_Step( &p_sw->Linear, Rot_vel ); // sets the steps to step for the linear stepper - Motor 1
    USB_Send_Msg("cf", 'S', &0.0, sizeof(0.0));
}