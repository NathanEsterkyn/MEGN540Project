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

static inline bool Right_XOR() // register PINE at pin PE6
{
    return bit_is_set(PINE, PE6 ); // returns if bit is 1 or 0
}
static inline bool Right_B() // register PINF at pin PF0
{
    return bit_is_set(PINF, PF0); // returns if bit is 1 or 0
}
static inline bool Right_A() // found by taking the Xor of B and Xor channels
{
    return Right_B() ^ Right_XOR();  // returns if bit is 1 or 0
}
static inline bool Left_XOR() // register PINB at pin PB4
{
    return bit_is_set( PINB, PB4 ); // returns if bit is 1 or 0
}
static inline bool Left_B() // register PINE at PE2
{
    return bit_is_set( PINE, PE2 ); // returns if bit is 1 or 0
}
static inline bool Left_A() // found by taking the Xor of B and Xor channels
{
    return Left_B() ^ Left_XOR(); // returns if bit is 1 or 0
}

/**
 * Function Encoders_Init initializes the encoders, sets up the pin change interrupts, and zeros the initial encoder
 * counts.
 */
void Initialize_Encoders()
{
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

    // setting up the pin change interrupt control register
    PCICR |= (1 << PCIE0); // bit 0 - enable pin change interrupt 0
    // setting up the pin change enable mask register
    PCMSK0 |= (1 << PCINT4); // bit 4 - enable pin change interrupt on pin 4
    // setting up the pin change interrupt flag register
    PCIFR |= (1 << PCIF0); // bit 0 - set to high when an interrupt request is detected
    // setting up the external interrupt flag register
    EIFR|= (INTF6); // bit 6 - flag raised when an interrupt is detected on INT6
    // setting up the external interrupt control register B
    EICRB |= (1 << ISC60); // bit 4 - Set INT6 to trigger on any logic change
    // setting up the external interrupt mask register
    EIMSK |= (1 << INT6);  // bit 6 - Enables pin INT6 to serve as the interrupt


    // Initialize static file variables
    _last_right_A = Right_A();
    _last_right_B = Right_B();

    _last_left_A   = Left_A();
    _last_left_B   = Left_A();
    _last_left_XOR = Left_XOR();
    _last_right_XOR = Right_XOR();

    _left_counts  = 0;
    _right_counts = 0;
}

/**
 * Function Encoder_Counts_Left returns the number of counts from the left encoder.
 * @return [int32_t] The count number.
 */
int32_t Encoder_Counts_Left()
{
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
    cli(); // Disable interrupts
    int32_t counts = _right_counts; // Copy the value of _right_counts
    sei(); // Re-enable interrupts
    return counts;
}

/**
 * Function Encoder_Rad_Left returns the number of radians for the left encoder.
 * @return [float] Encoder angle in radians
 */
float Encoder_Rad_Left()
{
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
    // Calculate the number of radians
    float radians = _right_counts * ( 2 * M_PI / 12 ); // 12 is the number of counts per rotation
    return radians;
}

/**
 * Interrupt Service Routine for the left Encoder. Note: May need to check that it is actually PCINT4 that triggered, as
 * the Pin Change Interrupts can trigger for multiple pins.
 * @return
 */
ISR( PCINT0_vect )
{
    /**    if( bit_is_set(PINB, PB4)){
            if(_last_left_XOR){
                if(bit_is_set(PINE, PE2)){
                    _left_counts++;
                }
                else{
                    _left_counts--;
                }
            }
            else{
                if(bit_is_set(PINE, PE2)){
                    _left_counts--;
                }
                else{
                    _left_counts++;
                }
            }
            _last_left_XOR = !_last_left_XOR;
        }
    }
    **/
    _left_counts += ( Left_A() ^ _last_left_B ) - ( _last_left_A ^ Left_B() );
    _last_left_B = Left_B();
    _last_left_A = Left_A();
}
/**
 * Interrupt Service Routine for the right Encoder.
 * @return
 */
ISR( INT6_vect )
{
    /**
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
    **/
    _right_counts += ( Right_A() ^ _last_right_B ) - ( _last_right_A ^ Right_B() );
    _last_right_B = Right_B();
    _last_right_A = Right_A();
}