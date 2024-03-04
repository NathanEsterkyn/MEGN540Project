#include "Battery_Monitor.h"

static const float BITS_TO_BATTERY_VOLTS = 0.0050049; // conversion factor = (2*2.56/1023);

/**
 * Function Initialize_Battery_Monitor initializes the Battery Monitor to record the current battery voltages.
 */
void Initialize_Battery_Monitor()
{
    // setting up the ADC control and status register A
    ADCSRA |= (1 << ADEN); // bit 7 - turns the ADC on
    ADCSRA |= (1 << ADPS2); // bit 2
    ADCSRA |= (1 << ADPS1); // bit 1
    ADCSRA |= (1 << ADPS0); // bit 0 - setting bits 2:0 to 1 sets division factor to 128
    // setting up the ADC multiplexer selection register
    ADMUX |= (1 << REFS1); // bit 7
    ADMUX |= (1 << REFS0); // bit 6 - setting bits 7:6 to 1: (internal 2.56V VRef with external cap on AREF)
    ADMUX |= (1 << MUX2); // bit 2
    ADMUX |= (1 << MUX1); // bit 1
    ADCSRB &= !(1 << MUX5); // bit 5

    // setting bits MUX1 and MUX2 to 1 and MUX5 to 0 results in a MUX5 value of 000110.
    // which selects a single input to the ADC coming from Pin f6 (ADC6). This is where
    // the Zumo sends it Vbat/2 reference voltage.
}

/**
 * Function Battery_Voltage initiates the A/D measurement and returns the result for the battery voltage.
 */
float Battery_Voltage()
{
    // A Union to assist with reading the LSB and MSB in the 16 bit register
    union {
        struct {
            uint8_t LSB;
            uint8_t MSB;
        } split;
        uint16_t value;
    } data = { .value = 0 };

    // *** MEGN540 LAB3 YOUR CODE HERE ***
    ADCSRA |= (1 << ADSC); // bit 6 - initiate the ADC reading - start conversion
    while(bit_is_set(ADCSRA,ADSC)) // while the ADC is reading bit 6 will remain 1, do nothing
    {}
    data.split.LSB = ADCL; // record least significant bit
    data.split.MSB = ADCH & 0b00000011; // record most significant bit
    return data.value * BITS_TO_BATTERY_VOLTS; // return the value in voltage
}
