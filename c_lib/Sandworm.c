#include "Sandworm.h"

void Initialize_Sandworm( Sandworm_t* p_sw, float Lin_pos, float Rot_pos, float Radius ) {

    p_sw->Lin_pos = Lin_pos; // set linear position
    p_sw->Rot_pos = Rot_pos; // set rotary position
    p_sw->Radius = Radius; // set radius of table

    Initialize_Stepper( &p_sw->Linear, 0, 200, 36 ); // create the linear stepper object - Motor 1
    Initialize_Stepper( &p_sw->Rotary, 0, 200, 8 ); // create the rotary stepper object - Motor 2

    // DDRB |= ( 1 << DDB0 ); // set up registers for limit switch, encoder, and main power button / LED
}

void Sandworm_Home( Sandworm_t* p_sw ) { // for testing I am sending it Speed (Lin_Vel) then Steps (Rot_Vel)
}

bool Sandworm_Limit( Sandworm_t* p_sw ) {
    // function to determine if button has been pressed
}