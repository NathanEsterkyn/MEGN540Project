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
    p_step->position = pos;
    p_step->number_of_steps = number_of_steps;
    p_step->motor_pin_1 = motor_pin_1;

    // Set registers for controlling the pins:
    if ( p_step->motor_pin_1 == 8 ) { // if the selected stepper is Motor 1
        // Set Data Direction Registers (DDR) for Port B
        DDRB |= ( 1 << DDB0 );  // bit 0
        DDRB |= ( 1 << DDB1 );  // bit 1
        DDRB |= ( 1 << DDB2 );  // bit 2
        DDRB |= ( 1 << DDB3 );  // bit 3 - sets PB 0,1,2,and 3 to outputs
    }

    if ( p_step->motor_pin_1 == 36 ) { // if the selected stepper is Motor 2
        // Set Data Direction Registers (DDR) for Port F
        DDRF |= ( 1 << DDF7 );  // bit 7
        DDRF |= ( 1 << DDF6 );  // bit 6
        DDRF |= ( 1 << DDF5 );  // bit 5
        DDRF |= ( 1 << DDF4 );  // bit 4 - sets PF 4,5,6,and 7 to outputs
    }
}
/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */
Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
                  int motor_pin_3, int motor_pin_4)
{
    this->step_number = 0;    // which step the motor is on
    this->direction = 0;      // motor direction
    this->last_step_time = 0; // timestamp in us of the last step taken
    this->number_of_steps = number_of_steps; // total number of steps for this motor

    // Arduino pins for the motor control connection:
    this->motor_pin_1 = motor_pin_1;
    this->motor_pin_2 = motor_pin_2;
    this->motor_pin_3 = motor_pin_3;
    this->motor_pin_4 = motor_pin_4;

    // setup the pins on the microcontroller:
    pinMode(this->motor_pin_1, OUTPUT);
    pinMode(this->motor_pin_2, OUTPUT);
    pinMode(this->motor_pin_3, OUTPUT);
    pinMode(this->motor_pin_4, OUTPUT);

    // When there are 4 pins, set the others to 0:
    this->motor_pin_5 = 0;

    // pin_count is used by the stepMotor() method:
    this->pin_count = 4;
}

/*
 * Sets the speed in revs per minute
 */
void Stepper::setSpeed(long whatSpeed)
{
    this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
}

/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */
void Stepper::step(int steps_to_move)
{
    int steps_left = abs(steps_to_move);  // how many steps to take

    // determine direction based on whether steps_to_mode is + or -:
    if (steps_to_move > 0) { this->direction = 1; }
    if (steps_to_move < 0) { this->direction = 0; }


    // decrement the number of steps, moving one step each time:
    while (steps_left > 0)
    {
        unsigned long now = micros();
        // move only if the appropriate delay has passed:
        if (now - this->last_step_time >= this->step_delay)
        {
            // get the timeStamp of when you stepped:
            this->last_step_time = now;
            // increment or decrement the step number,
            // depending on direction:
            if (this->direction == 1)
            {
                this->step_number++;
                if (this->step_number == this->number_of_steps) {
                    this->step_number = 0;
                }
            }
            else
            {
                if (this->step_number == 0) {
                    this->step_number = this->number_of_steps;
                }
                this->step_number--;
            }
            // decrement the steps left:
            steps_left--;
            // step the motor to step number 0, 1, ..., {3 or 10}
            if (this->pin_count == 5)
                stepMotor(this->step_number % 10);
            else
                stepMotor(this->step_number % 4);
        } else {
            yield();
        }
    }
}

/*
 * Moves the motor forward or backwards.
 */
void Stepper::stepMotor(int thisStep)
{
    if (this->pin_count == 2) {
        switch (thisStep) {
            case 0:  // 01
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, HIGH);
                break;
            case 1:  // 11
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, HIGH);
                break;
            case 2:  // 10
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, LOW);
                break;
            case 3:  // 00
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, LOW);
                break;
        }
    }
    if (this->pin_count == 4) {
        switch (thisStep) {
            case 0:  // 1010
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, LOW);
                digitalWrite(motor_pin_3, HIGH);
                digitalWrite(motor_pin_4, LOW);
                break;
            case 1:  // 0110
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, HIGH);
                digitalWrite(motor_pin_3, HIGH);
                digitalWrite(motor_pin_4, LOW);
                break;
            case 2:  //0101
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, HIGH);
                digitalWrite(motor_pin_3, LOW);
                digitalWrite(motor_pin_4, HIGH);
                break;
            case 3:  //1001
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, LOW);
                digitalWrite(motor_pin_3, LOW);
                digitalWrite(motor_pin_4, HIGH);
                break;
        }
    }

    if (this->pin_count == 5) {
        switch (thisStep) {
            case 0:  // 01101
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, HIGH);
                digitalWrite(motor_pin_3, HIGH);
                digitalWrite(motor_pin_4, LOW);
                digitalWrite(motor_pin_5, HIGH);
                break;
            case 1:  // 01001
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, HIGH);
                digitalWrite(motor_pin_3, LOW);
                digitalWrite(motor_pin_4, LOW);
                digitalWrite(motor_pin_5, HIGH);
                break;
            case 2:  // 01011
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, HIGH);
                digitalWrite(motor_pin_3, LOW);
                digitalWrite(motor_pin_4, HIGH);
                digitalWrite(motor_pin_5, HIGH);
                break;
            case 3:  // 01010
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, HIGH);
                digitalWrite(motor_pin_3, LOW);
                digitalWrite(motor_pin_4, HIGH);
                digitalWrite(motor_pin_5, LOW);
                break;
            case 4:  // 11010
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, HIGH);
                digitalWrite(motor_pin_3, LOW);
                digitalWrite(motor_pin_4, HIGH);
                digitalWrite(motor_pin_5, LOW);
                break;
            case 5:  // 10010
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, LOW);
                digitalWrite(motor_pin_3, LOW);
                digitalWrite(motor_pin_4, HIGH);
                digitalWrite(motor_pin_5, LOW);
                break;
            case 6:  // 10110
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, LOW);
                digitalWrite(motor_pin_3, HIGH);
                digitalWrite(motor_pin_4, HIGH);
                digitalWrite(motor_pin_5, LOW);
                break;
            case 7:  // 10100
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, LOW);
                digitalWrite(motor_pin_3, HIGH);
                digitalWrite(motor_pin_4, LOW);
                digitalWrite(motor_pin_5, LOW);
                break;
            case 8:  // 10101
                digitalWrite(motor_pin_1, HIGH);
                digitalWrite(motor_pin_2, LOW);
                digitalWrite(motor_pin_3, HIGH);
                digitalWrite(motor_pin_4, LOW);
                digitalWrite(motor_pin_5, HIGH);
                break;
            case 9:  // 00101
                digitalWrite(motor_pin_1, LOW);
                digitalWrite(motor_pin_2, LOW);
                digitalWrite(motor_pin_3, HIGH);
                digitalWrite(motor_pin_4, LOW);
                digitalWrite(motor_pin_5, HIGH);
                break;
        }
    }
}

/*
  version() returns the version of the library:
*/
int Stepper::version(void)
{
    return 5;
}
