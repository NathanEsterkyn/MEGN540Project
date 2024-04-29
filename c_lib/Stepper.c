#include "Stepper.h"
#include "Final_Tasks.h"
#include "SerialIO.h"

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

void Initialize_Stepper(Stepper_t *p_step, int number_of_steps,
                        int motor_pin_1) {
  // Fill stepper structure with relevant data
  p_step->number_of_steps = number_of_steps;
  p_step->motor_pin_1 = motor_pin_1;
  p_step->last_step_time = 0.0;
  p_step->direction = 1;
  p_step->step_number = 0;
  p_step->step_delay = 0.0;

  // Set DDR for both motors
  if (p_step->motor_pin_1 == 8) {
    DDRB |= 0x0F;
    PORTB = 0x00;
  } // sets PB 0,1,2,and 3 to outputs
  if (p_step->motor_pin_1 == 36) {
    DDRF |= 0xF0;
    PORTF = 0x00;
  } // sets PF 4,5,6,and 7 to outputs

  // Interrupts on Timer/Counter3 will trigger each step, setup timer1 and timer3:

  TCCR1A = 0x00; // sets bits to zero - Normal port operation, OC1A and OC1B disconnected (no output to ports)
  TCCR1B |= (1 << WGM12); // select mode 4 - clear timer on compare match
  TCCR1B |= (1 << CS12); // select a prescalar of 256 - 62.5 pulses per millisecond
  TIMSK1 &= ~(1 << OCIE1A); // disables output compare match with the OCR1A register

  TCCR3A = 0x00; // sets bits to zero - Normal port operation, OC1A and OC1B disconnected (no output to ports)
  TCCR3B |= (1 << WGM32); // select mode 4 - clear timer on compare match
  TCCR3B |= (1 << CS31); // select a prescalar of 8 - 2000 pulses per millisecond
  TIMSK3 &= ~(1 << OCIE3A); // disables output compare match with the OCR3A register

  OCR1A = 0; // initialize compare value: 2000 means the clock will clear and the ISR will run each millisecond
  OCR3A = 0; // initialize compare value: 2000 means the clock will clear and the ISR will run each millisecond

  PORTB = 0x00;
  PORTF = 0x00;
}

void Stepper_Speed(Stepper_t *p_step, float Value) {

  if (Value > 0.0) { // if the motor is moving positive direction
    p_step->direction = 1;
  }
  if (Value < 0.0) {
    p_step->direction = 0;
    Value = -Value;
  }

  float steps = p_step->number_of_steps; // converts number of steps per revolution into float
  steps = (Value / 60.0) * steps;
  p_step->step_delay =
      (1.0 / steps) * 1000.0; // returns milliseconds between steps to achieve desired speed in RPM

  if (p_step->motor_pin_1 == 8) { // motor 1 is on timer1 (rotary)
    uint32_t delay_time = 62.5 * p_step->step_delay; // converts to a 16 bit int to set registers
    OCR1A = delay_time;
  }
  if (p_step->motor_pin_1 == 36) { // motor 2 is on timer3 (linear)
    uint32_t delay_time = 2000 * p_step->step_delay; // converts to a 16 bit int to set registers
    OCR3A = delay_time;
  }
}

void Stepper_Step(Stepper_t *p_step) {
  // Determine stepper direction
  if (p_step->direction == 1) {
    p_step->step_number++;
    if (p_step->step_number == p_step->number_of_steps) {
      p_step->step_number = 0; // for rolling over
    }
  } else {
    if (p_step->step_number == 0) {
      p_step->step_number = p_step->number_of_steps;
    }
    p_step->step_number--;
  }

  // Step motor
  if (p_step->motor_pin_1 == 8) { // if the selected stepper is Motor 1
    switch (p_step->step_number % 4) {
    case 0:         // 1010
      PORTB = 0x0A; // sets PB 0,1,2,and 3
      break;
    case 1:         // 0110
      PORTB = 0x06; // sets PB 0,1,2,and 3
      break;
    case 2:         // 0101
      PORTB = 0x05; // sets PB 0,1,2,and 3
      break;
    case 3:         // 1001
      PORTB = 0x09; // sets PB 0,1,2,and 3
      break;
    }
  }
  if (p_step->motor_pin_1 == 36) { // if the selected stepper is Motor 2

    switch (p_step->step_number % 4) {
    case 0:         // 1010
      PORTF = 0xA0; // sets PF5 and PF7 to HIGH and all others to LOW
      break;
    case 1:         // 0110
      PORTF = 0x60; // sets PF5 and PF6 to HIGH and all others to LOW
      break;
    case 2:         // 0101
      PORTF = 0x50; // sets PF4 and PF6 to HIGH and all others to LOW
      break;
    case 3:         // 1001
      PORTF = 0x90; // sets PF4 and PF7 to HIGH and all others to LOW
      break;
    }
  }
}

void Stepper_Disable(Stepper_t *p_step) {
  if (p_step->motor_pin_1 == 8) { // if the selected stepper is Motor 1
    TIMSK1 &=
        ~(1 << OCIE1A); // disables output compare match with the OCR1A register
    PORTB = 0x00;
  }
  if (p_step->motor_pin_1 == 36) { // if the selected stepper is Motor 2
    TIMSK3 &=
        ~(1 << OCIE3A); // disables output compare match with the OCR1A register
    PORTF = 0x00;
  }
}

void Stepper_Enable(Stepper_t *p_step) {
  cli();
  if (p_step->motor_pin_1 == 8) { // if the selected stepper is Motor 1
    TIMSK1 |=
        (1 << OCIE1A); // disables output compare match with the OCR1A register
  }
  if (p_step->motor_pin_1 == 36) { // if the selected stepper is Motor 2
    TIMSK3 |=
        (1 << OCIE3A); // disables output compare match with the OCR1A register
  }
  sei();
}

ISR(TIMER1_COMPA_vect) // performs steps for motor 1 (rotary on PORTB)
{
  Stepper_Step(&Sandworm_Robot.Rotary); // steps rotary motor once
  if (Sandworm_Robot.Rotary.direction == 1) {
    Sandworm_Robot.Rot_pos++;
  } // increment position depending on direction
  if (Sandworm_Robot.Rotary.direction == 0) {
    Sandworm_Robot.Rot_pos--;
  } // mode is set to clear counter on compare match so no need to reset counter
    // value here
}

ISR(TIMER3_COMPA_vect) // performs steps for motor 2 (Linear on PORTF)
{
  Stepper_Step(&Sandworm_Robot.Linear); // steps linear motor once
  if (Sandworm_Robot.Linear.direction == 1) {
    Sandworm_Robot.Lin_pos++;
  } // increment position depending on direction
  if (Sandworm_Robot.Linear.direction == 0) {
    Sandworm_Robot.Lin_pos--;
  }
}