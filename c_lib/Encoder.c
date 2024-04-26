#define _USE_MATH_DEFINES

#include "Encoder.h"

#include <math.h>

// https://www.omc-stepperonline.com/nema-17-closed-loop-stepper-motor-16ncm-22-7oz-in-with-magnetic-encoder-1000ppr-4000cpr-17hs08-1004-me1k
#define CPR 4000

static volatile bool last_A = 0;
static volatile bool last_B = 0;
static volatile int32_t counts = 0;

static inline bool E_XOR() { return PIND & (1 << PIND3); }
static inline bool E_B() { return PINF & (1 << PINF0); }
static inline bool E_A() { return E_XOR() ^ E_B(); }

/**
 * Function Encoders_Init initializes the encoders, sets up the pin change
 * interrupts, and zeros the initial encoder counts.
 */
void Initialize_Encoders() {
  cli();

  // Any edge of INT6 generates an interrupt
  EICRB |= (1 << ISC60);
  EICRB &= ~(1 << ISC61);
  EIMSK |= (1 << INT3);

  // D3 and F0 are inputs
  DDRD &= ~(1 << DDD3);
  DDRF &= ~(1 << DDF0);

  last_A = 0;
  last_B = 0;

  counts = 0;

  sei();
}

/**
 * Function Encoder_Counts returns the number of counts from the encoder.
 * @return [int32_t] The count number.
 */
int32_t Encoder_Counts() {
  cli();
  int _counts = counts;
  sei();

  return _counts;
}
/**
 * Function Encoder_Rad returns the number of radians for the encoder.
 * @return [float] Encoder angle in radians
 */
float Encoder_Rad() { return Encoder_Counts() / CPR * 2 * M_PI; }

/**
 * Interrupt Service Routine for the Encoder.
 * @return
 */
ISR(INT6_vect) {
  counts += (E_A() ^ last_B) - (last_A ^ E_B());
  last_A = E_A();
  last_B = E_B();
}