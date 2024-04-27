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

bool Limit_Switch_Status() {
  sei();
  return Sandworm_Robot.limitState;
  cli();
}

ISR(INT0_vect) {
  Sandworm_Robot.limitState = PIND & (1 << PIND0);
  if (PIND & (1 << PIND0) &&
      Sandworm_Robot.Lin_vel < 0) { // if home button is pressed
    Stop_Step(0.0);                 // stop the motors
    Sandworm_Robot.Lin_pos = 0.0;   // set zero
  }
}