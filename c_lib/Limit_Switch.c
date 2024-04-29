#include "Limit_Switch.h"
#include "Final_Tasks.h"
#include <SerialIO.h>

void Initialize_Limit_Switch() {
  sei();

  // any edge of INT0 generates an interrupt request
  EICRA |= (1 << ISC00);
  EICRA &= ~(1 << ISC01);
  EIMSK |= (1 << INT0);

  // D0 is an input
  DDRD &= ~(1 << DDD0);

  // activate pullup resistor
  PORTD |= (1 << PD0);

  cli();
}

void Initialize_Power_Button() {
  sei();

  // any edge of INT2 generates an interrupt request
  EICRA |= (1 << ISC20);
  EICRA &= ~(1 << ISC21);
  EIMSK |= (1 << INT2);

  // D2 is an input
  DDRD &= ~(1 << PORTD2);

  // activate pullup resistor
  PORTD |= (1 << PORTD2);

  cli();
}

bool Limit_Switch_Status() {
  sei();
  return Sandworm_Robot.limitState;
  cli();
}

bool Power_Button_Status() {
  sei();
  return Sandworm_Robot.buttonState;
  cli();
}

ISR(INT0_vect) {
  Sandworm_Robot.limitState = PIND & (1 << PIND0);
  if (PIND & (1 << PIND0) &&
      Sandworm_Robot.Lin_vel < 0) { // if limit switch is pressed
    Stop_Step(0.0);                 // stop the motors
    Sandworm_Robot.Lin_pos = 0.0;   // set zero
  }
}

ISR(INT2_vect) {
  Sandworm_Robot.buttonState = PIND & (1 << PIND2);
  if ( Sandworm_Robot.buttonState == 0 ) {  // if power button is pressed
    PORTD |= (1 << PORTD1); // illuminate LED
  }
  else {
    PORTD &= ~(1 << PORTD1); // don't illuminate LED
    Stop_Step(0.0); // stop the motors
  }
}