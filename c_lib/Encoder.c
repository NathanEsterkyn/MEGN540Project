#include "Encoder.h"

/**
 * Internal counters for the Interrupts to increment or decrement as necessary.
 */
static volatile bool _last_right_A = 0;  // Static limits it's use to this file
static volatile bool _last_right_B = 0;  // Static limits it's use to this file

static volatile bool _last_left_A   = 0;  // Static limits it's use to this file
static volatile bool _last_left_B   = 0;  // Static limits it's use to this file
static volatile bool _last_left_XOR = 0;  // Necessary to check if PB4 triggered the ISR or not

static volatile int32_t _left_counts  = 0;  // Static limits it's use to this file
static volatile int32_t _right_counts = 0;  // Static limits it's use to this file

/** Helper Funcions for Accessing Bit Information */
// *** MEGN540 Lab 3 TODO ***
// Hint, use avr's bit_is_set function to help
static inline bool Right_XOR()
{
    return bit_is_set(PINE, PE6 ); // ????
}  // MEGN540 Lab
static inline bool Right_B()
{
    return bit_is_set(PINF, PF0); // ????
}  // MEGN540 Lab 3 TODO
static inline bool Right_A()
{
    return Right_B() ^ Right_XOR();  // ????
}  // MEGN540 Lab 3 TODO

static inline bool Left_XOR()
{
    return bit_is_set( PINB, PB4 ); // ????

}  // MEGN540 Lab 3 TODO
static inline bool Left_B()
{
    return bit_is_set( PINE, PE2 ); // ????

}  // MEGN540 Lab 3 TODO
static inline bool Left_A()
{
    return Left_B() ^ Left_XOR(); // ????
}  // MEGN540 Lab 3 TODO

/**
 * Function Encoders_Init initializes the encoders, sets up the pin change interrupts, and zeros the initial encoder
 * counts.
 */
void Initialize_Encoders()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE

    // Left encoder uses PB4 and PE2 pins as digital inputs. External interrupt PCINT4 is necessary to detect
    // the change in XOR flag. You'll need to see Section 11.1.5 - 11.1.7 for setup and use.
    // Note that the PCINT interrupt is trigered by any PCINT pin. In the ISR you should check to make sure
    // the interrupt triggered is the one you intend on processing.

    // Right encoder uses PE6 adn PF0 as digital inputs. External interrupt INT6 is necessary to detect
    // the changes in XOR flag. You'll need to see Sections 11.1.2-11.1.4 for setup and use.
    // You'll use the INT6_vect ISR flag.
    // DDRB &= ~(1 << PB4);  // Set PB4 as input
    // DDRE &= ~(1 << PE2);  // Set PE2 as input

    // DDRF &= ~(1 << PF0);  // Set PF0 as input
    // DDRE &= ~(1 << PE6);  // Set PE6 as input

    PCICR |= (1 << PCIE0);  // Enable Pin Change Interrupt 0
    PCMSK0 |= (1 << PCINT4);  // Enable Pin Change Interrupt 4
    PCIFR |=(1<<PCIF0);
    EIFR|=(INTF6);
    EICRB |= (1 << ISC60);  // Set INT6 to trigger on rising edge
    EIMSK |= (1 << INT6);  // Enable INT6


    // Initialize static file variables. These probably need to be updated.
    _last_right_A = Right_A();  // MEGN540 Lab 3 TODO
    _last_right_B = Right_B();  // MEGN540 Lab 3 TODO

    _last_left_A   = Left_A();  // MEGN540 Lab 3 TODO
    _last_left_B   = Left_A();  // MEGN540 Lab 3 TODO
    _last_left_XOR = Left_XOR();  // MEGN540 Lab 3 TODO

    _left_counts  = 0;  // MEGN540 Lab 3 TODO
    _right_counts = 0;  // MEGN540 Lab 3 TODO
}

/**
 * Function Encoder_Counts_Left returns the number of counts from the left encoder.
 * @return [int32_t] The count number.
 */
int32_t Encoder_Counts_Left()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE
    // Note: Interrupts can trigger during a function call and an int32 requires
    // multiple clock cycles to read/save. You may want to stop interrupts, copy the value,
    // and re-enable interrupts to prevent this from corrupting your read/write.
    cli();  // Disable interrupts

    int32_t counts = _left_counts;  // Copy the value of _left_counts

    sei();  // Re-enable interrupts

    return counts;
}

/**
 * Function Encoder_Counts_Right returns the number of counts from the right encoder.
 * @return [int32_t] The count number.
 */
int32_t Encoder_Counts_Right()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE
    // Note: Interrupts can trigger during a function call and an int32 requires
    // multiple clock cycles to read/save. You may want to stop interrupts, copy the value,
    // and re-enable interrupts to prevent this from corrupting your read/write.
    // Disable interrupts
    cli();

    // Copy the value of _right_counts
    int32_t counts = _right_counts;

    // Re-enable interrupts
    sei();

    return counts;
}

/**
 * Function Encoder_Rad_Left returns the number of radians for the left encoder.
 * @return [float] Encoder angle in radians
 */
float Encoder_Rad_Left()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE.  How many counts per rotation???
    // Calculate the number of radians
    float radians = _left_counts * ( 2 * M_PI / 12 ); // 12 is the number of counts per rotation
    return radians;
}

/**
 * Function Encoder_Rad_Right returns the number of radians for the left encoder.
 * @return [float] Encoder angle in radians
 */
float Encoder_Rad_Right()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE.  How many counts per rotation???
    // Calculate the number of radians
    float radians = _right_counts * ( 2 * M_PI / 12 );
    return radians;
}

/**
 * Interrupt Service Routine for the left Encoder. Note: May need to check that it is actually PCINT4 that triggered, as
 * the Pin Change Interrupts can trigger for multiple pins.
 * @return
 */
ISR(PCINT0_vect)
{
    if( bit_is_set(PINB, PB4))
    {
        if(_last_left_XOR)
        {
            if(bit_is_set(PINE, PE2))
            {
                _left_counts++;
            }
            else
            {
                _left_counts--;
            }
        }
        else
        {
            if(bit_is_set(PINE, PE2))
            {
                _left_counts--;
            }
            else
            {
                _left_counts++;
            }
        }
        _last_left_XOR = !_last_left_XOR;
    }
}

/**
 * Interrupt Service Routine for the right Encoder.
 * @return
 */
ISR(INT6_vect)
{
    if( bit_is_set( PINE, PE6 ) ) {
        if( _last_right_A ) {
            if( bit_is_set( PINF, PF0 ) ) {
                _right_counts++;
            } else {
                _right_counts--;
            }
        } else {
            if( bit_is_set( PINF, PF0 ) ) {
                _right_counts--;
            } else {
                _right_counts++;
            }
        }
        _last_right_A = !_last_right_A;
    }
}