#include "Sandworm.h"

#include <avr/io.h>

void Initialize_Sandworm(Sandworm_t *p_sw, float Lin_pos, float Rot_pos,float Radius) {

  // Fill stepper structure with relevant data
  p_sw->Lin_pos = Lin_pos; // set linear position
  p_sw->Rot_pos = Rot_pos; // set rotary position
  p_sw->Radius = Radius;   // set radius of table
  p_sw->buttonState = 1;
  p_sw->limitState = 0;
  p_sw->ledState = 0;

  // Set up pin change interrupts for power button and limit switch
  DDRD |= (1 << PORTD1); // data direction register for LED pin to output // ADDED
  DDRD &= ~(1 << DDD0); // data direction register for Limit Switch to input
  PORTD |= (1 << PD0);  // activate port d pull-up resistor on pd0

  // sets INT2 (power button) to trigger an interrupt on low // ADDED
  EICRA |= (1 << ISC20);
  EICRA &= ~(1 << ISC21);
  EIMSK |= (1 << INT2);   // enables interrupt on INT2

  // sets INT0 (limit switch) to trigger on any edge
  EICRA |= (1 << ISC00);
  EICRA &= ~(1 << ISC01);
  EIMSK |= (1 << INT0); // enables interrupt on INT0

  // Initialize Stepper motors on given pins
  Initialize_Stepper(&p_sw->Linear, 200, 36); // create the linear stepper object - Motor 1
  Initialize_Stepper(&p_sw->Rotary, 200, 8); // create the rotary stepper object - Motor 2
}