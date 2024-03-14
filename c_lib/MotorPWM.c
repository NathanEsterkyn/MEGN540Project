#include "MotorPWM.h"

/**
 * Function Initialize_Motor_PWM initializes the motor PWM on Timer 1 for PWM based voltage control of the motors.
 * The Motor PWM system shall initialize in the disabled state for safety reasons. You should specifically enable
 * Motor PWM outputs only as necessary.
 * @param [uint16_t] MAX_PWM is the maximum PWM value to use. This controls the PWM frequency.
 */
void Initialize_MotorPWM( uint16_t MAX_PWM )
{
    // Note:
    // the Timer Overflow Flag (TOV1) is set each time the counter reaches TOP - so is ICFn Flag (once per cycle)
    // Left motor PWM on PB6 with direction via PB2 - OC1B
    // Right motor PWM on PB5 with direction via PB1 - OC1A

    // Set Data Direction Register (DDR) for OC1A and OC1B
    DDRB |= ( 1 << DDB6 );  // bit 6
    DDRB |= ( 1 << DDB5 );  // bit 5
    DDRB |= ( 1 << DDB2 );  // bit 2
    DDRB |= ( 1 << DDB1 );  // bit 1 - sets PB 1,2,5,and 6 to outputs

    // Configure Timer1 waveform generation mode for phase and freq correct PWM - mode 8
    TCCR1B |= ( 1 << WGM13 );  // bit 4 - sets mode to phase and freq correct PWM with TOP stored at ICR1

    // Configure Timer1 clock select
    TCCR1B |= ( 1 << CS10 );  // bit 0 - No pre-scaling selected

    // Set the TOP value for base PWM freq.
    MotorPWM_Set_Max( MAX_PWM );
    MotorPWM_Enable( false );
}

/**
 * Function MotorPWM_Enable enables or disables the motor PWM outputs.
 * @param [bool] enable (true set enable, false set disable)
 */
void MotorPWM_Enable( bool enable )
{
    // Configure Timer1 compare output mode for phase and freq correct PWM - Clear OC1A and OC1B on compare match
    // when up-counting. Set OC1A and OC1B on compare match when down-counting. (for a non-inverted PWM signal)
    if( enable == 1 ) {
        TCCR1A |= ( 1 << COM1B1 ); // bit 5 - sets up channel B for this behavior
        TCCR1A |= ( 1 << COM1A1 ); // bit 7 - sets up channel A for this behavior
    }
    if( enable == 0 ) {
        TCCR1A &= ~( 1 << COM1B1 );
        TCCR1A &= ~( 1 << COM1A1 ); // sets bits to zero - Normal port operation, OC1A and OC1B disconnected
    }
}

/**
 * Function MotorPWM_Is_Enabled returns if the motor PWM is enabled for output.
 * @param [bool] true if enabled, false if disabled
 */
bool MotorPWM_Is_Enabled()
{
    if bit_is_set( TCCR1A, COM1A1 ) { // use bit_is_set function to access register state for OC1A
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
            OCR1B = ( pwm ); // sets output compare register, triggers interrupt when TCNT1 == pwm
        } else {
            PORTB |= ( 1 << PORTB2 );  // if pwm is negative - backwards
            OCR1B = -( pwm );
        }
    }
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
            OCR1A = ( pwm ); // sets output compare register, triggers interrupt when TCNT1 == pwm
        } else {
            PORTB |= ( 1 << PORTB1 );  // if pwm is negative - backwards
            OCR1A = -( pwm );
        }
    }
}

/**
 * Function MotorPWM_Get_Left returns the current PWM duty cycle for the left motor. If disabled it returns what the
 * PWM duty cycle would be.
 * @return [int16_t] duty-cycle for the left motor's pwm
 */
int16_t MotorPWM_Get_Left()
{
    int16_t pwm = OCR1B;
    int16_t TOP = MotorPWM_Get_Max();
    int16_t duty = (pwm/TOP)*100;
    return ( duty );
}

/**
 * Function MotorPWM_Get_Right returns the current PWM duty cycle for the right motor. If disabled it returns what the
 * PWM duty cycle would be.
 * @return [int16_t] duty-cycle for the right motor's pwm
 */
int16_t MotorPWM_Get_Right()
{
    int16_t pwm = OCR1A;
    int16_t TOP = MotorPWM_Get_Max();
    int16_t duty = (pwm/TOP)*100;
    return ( duty );
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
