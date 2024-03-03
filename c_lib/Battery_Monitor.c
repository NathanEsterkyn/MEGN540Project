#include "Battery_Monitor.h"

static const float BITS_TO_BATTERY_VOLTS = 0.0050049; //  2*2.56/1023;

/**
 * Function Initialize_Battery_Monitor initializes the Battery Monitor to record the current battery voltages.
 */
void Initialize_Battery_Monitor()
{

    ADCSRA |= (1 << ADPS2);
    ADCSRA |= (1 << ADPS1);
    ADCSRA |= (1 << ADPS0);
    ADCSRA |= (1 << ADEN);
    ADMUX |= (1 << REFS0);
    ADMUX |= (1 << REFS1);
    ADMUX |= (1 << MUX1);
    ADMUX |= (1 << MUX2);
    ADCSRB &= !(1 << MUX5);


    // *** MEGN540 LAB3 YOUR CODE HERE ***

}

/**
 * Function Battery_Voltage initiates the A/D measurement and returns the result for the battery voltage.
 */
float Battery_Voltage()
{
    // A Union to assist with reading the LSB and MSB in the  16 bit register
    union {
        struct {
            uint8_t LSB;
            uint8_t MSB;
        } split;
        uint16_t value;
    } data = { .value = 0 };


    // *** MEGN540 LAB3 YOUR CODE HERE ***
    ADCSRA |= (1 << ADSC);
    while(bit_is_set(ADCSRA,ADSC))
    {}
    data.split.LSB = ADCL;
    data.split.MSB = ADCH & 0b00000011;

    return data.value * BITS_TO_BATTERY_VOLTS;
}
