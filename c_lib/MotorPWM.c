#include "MotorPWM.h"

/**
 * Function Initialize_Motor_PWM initializes the motor PWM on Timer 1 for PWM based voltage control of the motors.
 * The Motor PWM system shall initialize in the disabled state for safety reasons. You should specifically enable
 * Motor PWM outputs only as necessary.
 * @param [uint16_t] MAX_PWM is the maximum PWM value to use. This controls the PWM frequency.
 */
void Initialize_MotorPWM( uint16_t MAX_PWM )
{
    // Set the Waveform Generation mode (WGMn3:0) and Compare Output mode (COMnx1:0) bits
    // fast PWM mode (WGMn3:0 = 5, 6, 7, 14, or 15)
    // In non-inverting Compare Output mode, the Output Compare (OC1x) is set on the compare
    // match between TCNTn and OCRnx, and cleared at TOP. In inverting Compare Output mode
    // output is cleared on compare match and set at TOP
    //
    // The PWM resolution for fast PWM can be fixed to 8-, 9-, or 10-bit, or defined by ICR1 (why aren't we using OCR1A with the double buffer?)
    // ICR1 sets TOP - ICR1, WGMn3:0 = 14 (1110), this is the base PWM freq.
    // TCNT1
    // OC1x Interrupt Flag will be set when a compare match occurs
    // The Timer/Counter Overflow Flag (TOVn) is set each time the counter reaches TOP - so is ICFn Flag
    // Left motor PWM on PB6 with direction via PB2 - OC1B
    // Right motor PWM on PB5 with direction via PB1 - OC1A

    // Set Data Direction Register (DDR) for OC1A and OC1B
    DDRB |= ( 1 << DDB6 );  // bit 6 // MODDED - keep disabled by setting as input
    DDRB |= ( 1 << DDB5 );  // bit 5 // MODDED - keep disabled by setting as input
    DDRB |= ( 1 << DDB2 );  // bit 2
    DDRB |= ( 1 << DDB1 );  // bit 1 - sets PB 1,2,5,and 6 to outputs

    // Initialize Timer1
    // TCNT1 = 0x0000; // bit x - initialize counter // update this to work with 16 bit registers page 113

    // Configure Timer1 compare output mode for phase and freq correct PWM
    // TCCR1A |= (1 << COM1B0); // bit 4
    // TCCR1B |= ( 1 << COM1B1 );  // bit 5 - overrides normal port functionality of PB6
    // TCCR1A |= (1 << COM1A0); // bit 6
    // TCCR1B |= ( 1 << COM1A1 );  // bit 7 - overrides normal port functionality of PB5

    // Configure Timer1 waveform generation mode for phase and freq correct PWM
    TCCR1B |= ( 1 << WGM13 );  // bit 4
    // TCCR1B |= ( 0 << WGM12 );  // bit 3
    // TCCR1A |= ( 0 << WGM11 );  // bit 1 - sets mode to phase and freq correct PWM with TOP stored at ICR1
    // TCCR1A |= ( 0 << WGM10 );  // bit 0

    // Configure Timer1 clock select
    // TCCR1B |= ( 0 << CS12 );  // bit 2
    // TCCR1B |= ( 0 << CS11 );  // bit 1
    TCCR1B |= ( 1 << CS10 );  // bit 0 - No pre-scaling selected

    // Set the TOP value for base PWM freq.
    MotorPWM_Set_Max( MAX_PWM );
    MotorPWM_Enable( false );

    // Ensure Timer1 is on
    // PRR0 |= (0 << PRTIM1); // bit 3
}

/**
 * Function MotorPWM_Enable enables or disables the motor PWM outputs.
 * @param [bool] enable (true set enable, false set disable)
 */
void MotorPWM_Enable( bool enable )
{
    if( enable == 1 ) {
        // DDRB |= ( 1 << DDB6 );  // bit 6
        // DDRB |= ( 1 << DDB5 );  // bit 5 - set data direction to output for PB5 and PB6
        TCCR1A |= ( 1 << COM1B1 );
        TCCR1A |= ( 1 << COM1A1 );
    }
    if( enable == 0 ) {
        // DDRB |= ( 0 << DDB6 );  // bit 6
        // DDRB |= ( 0 << DDB5 );  // bit 5 - set data direction to input for PB5 and PB6
        TCCR1A &= ~( 1 << COM1B1 );
        TCCR1A &= ~( 1 << COM1A1 );
    }
}

/**
 * Function MotorPWM_Is_Enabled returns if the motor PWM is enabled for output.
 * @param [bool] true if enabled, false if disabled
 */
bool MotorPWM_Is_Enabled()
{
    // detect if enabled
    // if( bit_is_set( DDRB, DDB6 ) && bit_is_set( DDRB, DDB5 ) ) {  // use bit_is_set function to access register state
    if bit_is_set( TCCR1A, COM1A1 ) {
        return true;
    } else {
        return false;
    }
}

/**
 * Function MotorPWM_Set_Left sets the PWM duty cycle for the left motor.
 * @return [int32_t] The count number.
 */
void MotorPWM_Set_Left( int16_t pwm )
{
    if( MotorPWM_Is_Enabled() ) {
        if( pwm > 0 ) {                 // detect if desired motion is forwards or backwards
            PORTB &= ~( 1 << PORTB2 );  // if pwm is positive - forwards
            OCR1B = ( pwm );
        } else {
            PORTB |= ( 1 << PORTB2 );  // if pwm is negative - backwards
            OCR1B = -( pwm );
        }
        // set the duty cycle here on PB6 - OC1B
        // uint16_t duty = ( pwm / MAX_DUTY ) * MotorPWM_Get_Max();
        // OCR1B         = duty;  // if OCR1B = TOP we get 100% duty cycle
    }
    // PORTB |= (1 << PORTB6); // access the data at PB5
}

/**
 * Function MototPWM_Set_Right sets the PWM duty cycle for the right motor.
 * @return [int32_t] The count number.
 */
void MotorPWM_Set_Right( int16_t pwm )
{
    if( MotorPWM_Is_Enabled() ) {
        if( pwm > 0 ) {                 // detect if desired motion is forwards or backwards
            PORTB &= ~( 1 << PORTB1 );  // if pwm is positive - forwards
            OCR1A = ( pwm );
        } else {
            PORTB |= ( 1 << PORTB1 );  // if pwm is negative - backwards
            OCR1A = -( pwm );
        }
        // set the duty cycle here on PB5 - OC1A
        // uint16_t duty = ( pwm / MAX_DUTY ) * MotorPWM_Get_Max();
        // OCR1A         = duty;  // if OCR1A = TOP we get 100% duty cycle
    }
    // PORTB |= (1 << PORTB5); // access the data at PB5
}

/**
 * Function MotorPWM_Get_Left returns the current PWM duty cycle for the left motor. If disabled it returns what the
 * PWM duty cycle would be.
 * @return [int16_t] duty-cycle for the left motor's pwm
 */
int16_t MotorPWM_Get_Left()
{
    return ( ( OCR1B / MotorPWM_Get_Max() ) * 100 );
}

/**
 * Function MotorPWM_Get_Right returns the current PWM duty cycle for the right motor. If disabled it returns what the
 * PWM duty cycle would be.
 * @return [int16_t] duty-cycle for the right motor's pwm
 */
int16_t MotorPWM_Get_Right()
{
    return ( ( OCR1A / MotorPWM_Get_Max() ) * 100 );
}

/**
 * Function MotorPWM_Get_Max() returns the PWM count that corresponds to 100 percent duty cycle (all on), this is the
 * same as the value written into ICR1 as (TOP).
 */
uint16_t MotorPWM_Get_Max()
{
    cli();               // Disable interrupts
    int16_t TOP = ICR1;  // get 16 bit value out of ICR1
    sei();               // Re-enable interrupts
    return TOP;
}

/**
 * Function MotorPWM_Set_Max sets the maximum pwm count. This function sets the timer counts to zero because
 * the ICR1 can cause undesired behaviors if change dynamically below the current counts.  See page 128 of the
 * atmega32U4 datasheat.
 */
void MotorPWM_Set_Max( uint16_t MAX_PWM )
{
    cli();           // Disable interrupts
    ICR1 = MAX_PWM;  // set 16 bit register ICR1 to max PWM
    sei();           // Re-enable interrupts
}
