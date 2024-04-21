#include "Stepper.h"

/*
 * The sequence of control signals for 4 control wires is as follows:
 *
 * Step C0 C1 C2 C3
 *    1  1  0  1  0
 *    2  0  1  1  0
 *    3  0  1  0  1
 *    4  1  0  0  1
 *
 *
 * Motor 1 and 2 pinouts as follows:
 * Motor 1
 * Arduino - TB6612 - ATMEGA - GPIO(INTERRUPT)
 *   MP1   -  AIN2  -  Pin 8 - PB0(PCINT0)
 *   MP2   -  AIN1  -  Pin 9 - PB1(PCINT1)
 *   MP3   -  BIN1  - Pin 10 - PB2(PCINT2)
 *   MP4   -  BIN2  - Pin 11 - PB3(PCINT3)
 * Motor 2
 * Arduino - TB6612 - ATMEGA - GPIO(INTERRUPT)
 *   MP1   -  AIN2  - Pin 36 - PF7(ADC7)
 *   MP2   -  AIN1  - Pin 37 - PF6(ADC6)
 *   MP3   -  BIN1  - Pin 38 - PF5(ADC5)
 *   MP4   -  BIN2  - Pin 39 - PF4(ADC4)
 */

void Initialize_Stepper( Stepper_t* p_step, uint16_t pos, int number_of_steps, int motor_pin_1 ) {

    // Fill stepper structure with relevant data
    rb_initialize_F(&p_step->position);
    rb_push_back_F(&p_step->position,pos);
    p_step->number_of_steps = number_of_steps;
    p_step->motor_pin_1 = motor_pin_1;
    p_step->last_step_time = 0.0;
    p_step->direction = 1;
    p_step->step_number = 0;
    p_step->step_delay = 0.0;

    // Set registers for controlling the pins:
    if ( p_step->motor_pin_1 == 8 ) { // if the selected stepper is Motor 1
        // Set Data Direction Registers (DDR) for Port B
        DDRB |= ( 1 << DDB0 );  // bit 0
        DDRB |= ( 1 << DDB1 );  // bit 1
        DDRB |= ( 1 << DDB2 );  // bit 2
        DDRB |= ( 1 << DDB3 );  // bit 3 - sets PB 0,1,2,and 3 to outputs
        //DDRB = (DDRB & 0xF0) | (1<<DDB0) | (1<<DDB1) | (1<<DDB2) | (1<<DDB3); // sets PF 4,5,6,and 7 to outputs
    }

    if ( p_step->motor_pin_1 == 36 ) { // if the selected stepper is Motor 2
        // Set Data Direction Registers (DDR) for Port F
        DDRF |= ( 1 << DDF7 );  // bit 7
        DDRF |= ( 1 << DDF6 );  // bit 6
        DDRF |= ( 1 << DDF5 );  // bit 5
        DDRF |= ( 1 << DDF4 );  // bit 4 - sets PF 4,5,6,and 7 to outputs
        //DDRF = (DDRF & 0xF0) | (1<<DDF7) | (1<<DDF6) | (1<<DDF5) | (1<<DDF4); // sets PF 4,5,6,and 7 to outputs
    }
}

void Stepper_Speed( Stepper_t* p_step, float Value) {
    float steps = p_step->number_of_steps; // converts number of steps per revolution into float
    steps = ( Value / 60.0 ) * steps;
    p_step->step_delay =  ( 1.0 / steps ) * 1000.0; // returns milliseconds between steps to achieve desired speed in RPM

    //float ret_val = p_step->step_delay; // FOR TESTING
    //USB_Send_Msg( "cf", 's', &ret_val, sizeof( ret_val ) ); // FOR TESTING
}

void Stepper_Step( Stepper_t* p_step ) {

    // Determine stepper direction
    /*
    if ( p_step->direction == 1 ) {
        p_step->step_number++;
        if ( p_step->step_number == p_step->number_of_steps) {
            p_step->step_number = 0; // for rolling over
        }
    }
    else {
        if ( p_step->step_number == 0 ) {
            p_step->step_number = p_step->number_of_steps;
        }
        p_step->step_number--;
    }
    */

    p_step->step_number++;
    if ( p_step->step_number == p_step->number_of_steps) {
        p_step->step_number = 0; // for rolling over
    }

    // Step motor
    if (p_step->motor_pin_1 == 8) { // if the selected stepper is Motor 1
        switch (p_step->step_number % 4) {
            case 0:  // 1010
                PORTB = (PORTB & 0xF0) | (1<<PORTB0) | (1<<PORTB2); // sets PB 0,1,2,and 3
                break;
            case 1:  // 0110
                PORTB = (PORTB & 0xF0) | (1<<PORTB1) | (1<<PORTB2); // sets PB 0,1,2,and 3
                break;
            case 2:  //0101
                PORTB = (PORTB & 0xF0) | (1<<PORTB1) | (1<<PORTB3); // sets PB 0,1,2,and 3
                break;
            case 3:  //1001
                PORTB = (PORTB & 0xF0) | (1<<PORTB0) | (1<<PORTB3); // sets PB 0,1,2,and 3
                break;
        }
    }
    if (p_step->motor_pin_1 == 36) { // if the selected stepper is Motor 2
        float ret_val = p_step->step_number; // FOR TESTING
        USB_Send_Msg( "cf", 'L', &ret_val, sizeof( ret_val ) ); // FOR TESTING
        //PORTF = 0x00; // turns off all ports
        switch (p_step->step_number % 4) {
            case 0:  // 1010
                PORTF = |0xA0 &~0x10;
                //PORTF |= 0xA0;
                //PORTF = (PORTF & 0xF0) | (1<<PORTF7) | (1<<PORTF5); // sets PF 4,5,6,and 7
                break;
            case 1:  // 0110
                PORTF = |0x60 &~0x80;
                //PORTF |= 0x60;
                //PORTF = (PORTF & 0xF0) | (1<<PORTF6) | (1<<PORTF5); // sets PF 4,5,6,and 7
                break;
            case 2:  //0101
                PORTF = |0x50 &~0x20;
                //PORTF |= 0x50;
                //PORTF = (PORTF & 0xF0) | (1<<PORTF6) | (1<<PORTF4); // sets PF 4,5,6,and 7
                break;
            case 3:  //1001
                PORTF = |0x90 &~0x40;
                //PORTF |= 0x90;
                //PORTF = (PORTF & 0xF0) | (1<<PORTF7) | (1<<PORTF4); // sets PF 4,5,6,and 7
                break;
        }
    }

}